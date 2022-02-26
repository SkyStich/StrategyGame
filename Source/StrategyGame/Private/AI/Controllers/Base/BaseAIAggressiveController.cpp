// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/Base/BaseAIAggressiveController.h"
#include "AI/Pawns/Base/BaseAggressivePawn.h"
//#include "Perception/AIPerceptionComponent.h"

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
	if(bOrderExecuted) return;
	if(!Actor->GetClass()->ImplementsInterface(UFindObjectTeamInterface::StaticClass())) return;
	
	if(Stimulus.WasSuccessfullySensed())
	{
		if(TargetActor)
		{
			if(TargetActor == Actor) GetWorld()->GetTimerManager().ClearTimer(LoseTargetHandle);
			return;
		}
		if(IFindObjectTeamInterface::Execute_FindObjectTeam(Actor) == GetAggressivePawn()->GetTeam()) return;
		
		TargetActor = Actor;
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

	GetWorld()->GetTimerManager().SetTimer(CheckDistanceForAttackHandle, this, &ABaseAIAggressiveController::CheckDistanceForAttack, 2.f, true);
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
		AggressivePawn->StartAttackTargetActor(TargetActor);
	}
}

void ABaseAIAggressiveController::StopCheckDistanceForAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(CheckDistanceForAttackHandle);
}

void ABaseAIAggressiveController::MoveToGiveOrder(const FVector& Location, AActor* NewTargetActor)
{
	Super::MoveToGiveOrder(Location, NewTargetActor);

	if(NewTargetActor)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Target is valid"));
		StartCheckDistanceForAttack();
	}
}


