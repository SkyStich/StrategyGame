// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/Base/BaseAIAggressiveController.h"
#include "AI/Pawns/Base/BaseAggressivePawn.h"
#include "Perception/AISenseConfig_Sight.h"

ABaseAIAggressiveController::ABaseAIAggressiveController(const FObjectInitializer& ObjectInitializer)
{
	SenseConfigInit();	
	
	LoseTargetTime = 8.f;
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
	StartLoseTimer();
}

ABaseAggressivePawn* ABaseAIAggressiveController::GetAggressivePawn() const
{
	return GetPawn() ? Cast<ABaseAggressivePawn>(GetPawn()) : nullptr;
}

void ABaseAIAggressiveController::StartLoseTimer()
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]() -> void
	{
		GetWorld()->GetTimerManager().ClearTimer(LoseTargetHandle);
		TargetActor = nullptr;
		StopCheckDistanceForAttack();
		StopMovement();
	});
	GetWorld()->GetTimerManager().SetTimer(LoseTargetHandle, TimerDelegate, LoseTargetTime, false);
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
	PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

	if(PerceivedActors.Num() <= 0) return;

	for(auto& ByArray : PerceivedActors)
	{
		if(!ByArray->CanBeDamaged()) continue;
		if(!ByArray->GetClass()->ImplementsInterface(UFindObjectTeamInterface::StaticClass())) continue;

		if(IFindObjectTeamInterface::Execute_FindObjectTeam(ByArray) == IFindObjectTeamInterface::Execute_FindObjectTeam(GetPawn())) continue;

		TargetActor = ByArray;
		MoveToActor(TargetActor, 15.f);
		StartCheckDistanceForAttack();
		return;
	}
}
