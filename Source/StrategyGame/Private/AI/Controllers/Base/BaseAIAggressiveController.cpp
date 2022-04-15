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
	if(!Actor || !Actor->GetClass()->ImplementsInterface(UFindObjectTeamInterface::StaticClass())) return;
	if(IFindObjectTeamInterface::Execute_FindObjectTeam(Actor) == GetAggressivePawn()->GetTeam()) return;
	
	/** if pawn can attack with movement */
	if(GetPathFollowingComponent()->GetStatus() == EPathFollowingStatus::Moving)
	{
		//@todo add logic if pawn can attack with movement
		return;
	}

	/** if sensed success */
	if(Stimulus.WasSuccessfullySensed())
	{
		/**
		 *if controller have target, check this target on eq with sense actor.
		 *if TargetActor eq with actor clear lose handle and start distance for attack
		 */
		if(TargetActor)
		{
			if(TargetActor == Actor)
			{
				GetWorld()->GetTimerManager().ClearTimer(LoseTargetHandle);
				StartCheckDistanceForAttack();
				return;
			}

			/**
			 * if the AI has a filter, we check the current object for its absence and the new object for its presence,
			 * if all checks are correct, change the current goal
			 */
			if(!GetFilterName().IsNone() && !TargetActor->Tags.Contains(GetFilterName()) && Actor->Tags.Contains(GetFilterName()))
			{
				MoveToGiveOrder(Actor->GetActorLocation(), Actor);
				StartCheckDistanceForAttack();
			}
			return;
		}
		MoveToGiveOrder(Actor->GetActorLocation(), Actor);
		StartCheckDistanceForAttack();
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
	
	StopCheckDistanceForAttack();
	
	if(NewTargetActor)
	{
		float const Distance = FVector::Dist(GetPawn()->GetActorLocation(), NewTargetActor->GetActorLocation());
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

	MoveToActor(TargetActor, 15);
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
	TArray<AActor*> PerceivedActors;
	if(!GetPerceptionComponent()->GetFilteredActors([&](FActorPerceptionInfo Item) -> bool
	{
		AActor* Actor = Item.Target.Get();
		if(!IsValid(Actor)) return false;
		
		bool const IsEnemyTeam = IFindObjectTeamInterface::Execute_FindObjectTeam(Actor) != GetAggressivePawn()->GetTeam();
		return Actor->CanBeDamaged() && IsEnemyTeam;
	}, PerceivedActors)) return false;
	
	
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
		MoveToGiveOrder(TempTarget->GetActorLocation(), TempTarget);
		StartCheckDistanceForAttack();
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
	MoveToGiveOrder(TempTarget->GetActorLocation(), TempTarget);
	StartCheckDistanceForAttack();
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
