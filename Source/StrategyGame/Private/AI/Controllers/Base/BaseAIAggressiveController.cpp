// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/Base/BaseAIAggressiveController.h"
#include "AI/Pawns/Base/BaseAggressivePawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AISenseConfig_Sight.h"

ABaseAIAggressiveController::ABaseAIAggressiveController(const FObjectInitializer& ObjectInitializer)
{
	SenseConfigInit();
}

void ABaseAIAggressiveController::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() == ROLE_Authority)
	{
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseAIAggressiveController::OnSinglePerceptionUpdated);
	}
}

void ABaseAIAggressiveController::OnSinglePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if(!Actor || !Actor->GetClass()->ImplementsInterface(UFindObjectTeamInterface::StaticClass())
		|| IFindObjectTeamInterface::Execute_FindObjectTeam(Actor) == GetAggressivePawn()->GetTeam()) return;
	
	/** if sensed success */
	if(Stimulus.WasSuccessfullySensed())
	{

		/** if TargetActor eq with actor clear lose handle and start distance for attack */
		if(TargetActor == Actor)
		{
			GetWorld()->GetTimerManager().ClearTimer(LoseTargetHandle);
			StartCheckDistanceForAttack();
			return;
		}

		/** if pawn can attack with movement WITH OUT CURRENT TARGET */
		if(GetPathFollowingComponent()->GetStatus() == EPathFollowingStatus::Moving)
		{
			//@todo add logic if pawn can attack with movement
			return;
		}
		
		if(!TargetActor)
		{
			if(OrderType != EOrderType::OrderExecuted) return;
			MoveToGiveOrder(Actor->GetActorLocation(), Actor);
			return;
		}
		
		/**
		* if the AI has a filter, we check the current object for its absence and the new object for its presence,
		* if all checks are correct, change the current goal
		*/
		if(!GetFilterName().IsNone() && !TargetActor->Tags.Contains(GetFilterName()) && Actor->Tags.Contains(GetFilterName()))
		{
			MoveToGiveOrder(Actor->GetActorLocation(), Actor);
		}
		return;
	}

	/** if perceived actor eq TargetActor */
	if(Actor == TargetActor)
	{
		if(OrderType == EOrderType::MoveToTarget)
		{
			StartChasingTarget();
			return;
		}
		FindNewTarget();
	}
}

ABaseAggressivePawn* ABaseAIAggressiveController::GetAggressivePawn() const
{
	return GetPawn() ? Cast<ABaseAggressivePawn>(GetPawn()) : nullptr;
}

void ABaseAIAggressiveController::StartLoseTimer()
{
	if(!TargetActor)
	{
		GetWorld()->GetTimerManager().ClearTimer(LoseTargetHandle);
		StopCheckDistanceForAttack();
		return;
	}
	
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]() -> void
	{
		GetWorld()->GetTimerManager().ClearTimer(LoseTargetHandle);
		TargetActor = nullptr;
		StopCheckDistanceForAttack();
		StopMovement();
	});
	GetWorld()->GetTimerManager().SetTimer(LoseTargetHandle, TimerDelegate, 6.f, false);
}

void ABaseAIAggressiveController::StartCheckDistanceForAttack()
{ 
	if(GetLocalRole() != ROLE_Authority || !TargetActor) return;
	GetWorld()->GetTimerManager().SetTimer(CheckDistanceForAttackHandle, this, &ABaseAIAggressiveController::CheckDistanceForAttack, 0.5f, true, 0);
}

void ABaseAIAggressiveController::CheckDistanceForAttack()
{
	if(!TargetActor)
	{
		StopCheckDistanceForAttack();
		return;
	}
	
	ABaseAggressivePawn* AggressivePawn = GetAggressivePawn();
	if(!AggressivePawn) return;

	float const Distance = FVector::Dist(AggressivePawn->GetActorLocation(), TargetActor->GetActorLocation());

	if(Distance <= AggressivePawn->GetAggressiveAttackData()->BaseDistanceForAttack)
	{
		StopMovement();
		AggressivePawn->StartAttackTargetActor(TargetActor);
		StopCheckDistanceForAttack();
	}
}

void ABaseAIAggressiveController::StopCheckDistanceForAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(CheckDistanceForAttackHandle);
}

void ABaseAIAggressiveController::MoveToGiveOrder(const FVector& Location, AActor* NewTargetActor)
{
	Super::MoveToGiveOrder(Location, NewTargetActor);

	if(GetLocalRole() != ROLE_Authority) return;
	
	StopCheckDistanceForAttack();
	
	if(TargetActor)
	{
		float const Distance = FVector::Dist(GetPawn()->GetActorLocation(), TargetActor->GetActorLocation());
		if(Distance <= SightConfig->SightRadius)
		{
			StartCheckDistanceForAttack();
		}
	}
}

void ABaseAIAggressiveController::StartChasingTarget()
{
	if(!TargetActor)
	{
		if(OrderType != EOrderType::MoveToTarget)
		{
			/** if can find > 1 target return, else chasing old target */
			if(FindNewTarget()) return;
		}
	}

	MoveToActor(TargetActor, 25);
	StartCheckDistanceForAttack();

	/** Limitation Check for Pursuit Range */
	if(MaxPursuitDistance <= 0 || GetWorld()->GetTimerManager().IsTimerActive(CheckPursuitDistanceHandle)) return;

	/** verification of reaching a rapid distance of infection */
	GetWorld()->GetTimerManager().SetTimer(CheckPursuitDistanceHandle, this, &ABaseAIAggressiveController::CheckHoldingDistance, 1.f, true);
}

void ABaseAIAggressiveController::StopChasingTarget()
{
	if(GetLocalRole() == ROLE_Authority)
	{
		StopMovement();
	}
}

bool ABaseAIAggressiveController::FindNewTarget()
{
	if(!PerceptionComponent->IsActive()) return false;

	TArray<AActor*> PerceivedActors;
	
	GetPerceptionComponent()->GetKnownPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
	PerceivedActors.RemoveAll([&](AActor* Item) -> bool
	{
	if(!IsValid(Item)) return false;
		
    bool const IsEnemyTeam = IFindObjectTeamInterface::Execute_FindObjectTeam(Item) != GetAggressivePawn()->GetTeam();
    return !Item->CanBeDamaged() && !IsEnemyTeam;
	});
	
	if(PerceivedActors.Num() <= 0) return false;
	
	Algo::Sort(PerceivedActors, [&](AActor* First, AActor* Second) -> bool
    {
		float const FirstDist = FVector::Dist(GetPawn()->GetActorLocation(), First->GetActorLocation());
		float const SecondDist = FVector::Dist(GetPawn()->GetActorLocation(), Second->GetActorLocation());
        return FirstDist < SecondDist;
    });

	auto TempTarget = PerceivedActors[0];
	
	/** if the AI does not have a filter, select the nearest target and give it a command */
	if(GetFilterName().IsNone())
	{
		MoveToActor(TempTarget);
		TargetActor = TempTarget;
		StartCheckDistanceForAttack();
		ForceNetUpdate();
		return true;
	}

	/** if we have a filter, we are looking for the nearest object that suffocates it,
	 * if this object is not in the array, we take the closest object and give it to the AI comund */
	for(const auto& ByArray : PerceivedActors)
	{
		if(ByArray->Tags.Contains(GetFilterName()))
		{
			TempTarget = ByArray;
			break;
		}
	}
	MoveToActor(TempTarget);
	TargetActor = TempTarget;
	StartCheckDistanceForAttack();
	ForceNetUpdate();
	return true;
}

void ABaseAIAggressiveController::OnBuildDestroyedTypeActive()
{
	Super::OnBuildDestroyedTypeActive();

	FindNewTarget();
}

void ABaseAIAggressiveController::OnWaitCommandTypeActive()
{
	Super::OnWaitCommandTypeActive();

	GetAggressivePawn()->StopAttack();
}
