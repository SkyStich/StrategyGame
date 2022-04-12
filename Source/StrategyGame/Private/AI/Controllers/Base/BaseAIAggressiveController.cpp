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
	if(!Actor || bOrderExecuted) return;
	if(!Actor->GetClass()->ImplementsInterface(UFindObjectTeamInterface::StaticClass())) return;
	
	if(Stimulus.WasSuccessfullySensed())
	{
		if(TargetActor)
		{
			if(TargetActor == Actor)
			{
				GetWorld()->GetTimerManager().ClearTimer(LoseTargetHandle);
				StartCheckDistanceForAttack();
			}
			return;
		}
		if(IFindObjectTeamInterface::Execute_FindObjectTeam(Actor) == GetAggressivePawn()->GetTeam()) return;
		
		MoveToGiveOrder(Actor->GetActorLocation(), Actor);
		StartCheckDistanceForAttack();
 
		return;
	}
	
	if(Actor != TargetActor) return;

	FindNewTarget();
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
	if(!TargetActor) return;

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

void ABaseAIAggressiveController::FindNewTarget()
{
	TArray<AActor*> PerceivedActors;
	GetPerceptionComponent()->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);
	if(PerceivedActors.Num() > 0)
	{
		Algo::Sort(PerceivedActors, [&](AActor* First, AActor* Second) -> bool
        {
            return FVector::Dist(GetPawn()->GetActorLocation(), First->GetActorLocation()) < FVector::Dist(GetPawn()->GetActorLocation(), Second->GetActorLocation());
        });

		for(const auto& ByArray : PerceivedActors)
		{
			if(IFindObjectTeamInterface::Execute_FindObjectTeam(ByArray) == GetAggressivePawn()->GetTeam()
				|| !ByArray->GetClass()->ImplementsInterface(UFindObjectTeamInterface::StaticClass())
				|| !ByArray->CanBeDamaged()) continue;
			
			TargetActor = ByArray;
			break;
		}
		if(TargetActor)
		{
			MoveToGiveOrder(TargetActor->GetActorLocation(), TargetActor);
			StartCheckDistanceForAttack();
		}
	}
}
