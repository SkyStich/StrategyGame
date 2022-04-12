// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Controllers/Base/BaseAIController.h"

#include "DrawDebugHelpers.h"
#include "GeneratedCodeHelpers.h"
#include "ActorComponents/Actors/ObjectHealthComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Net/UnrealNetwork.h"

ABaseAIController::ABaseAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	bAttachToPawn = true;
	bReplicates = true;
	NetUpdateFrequency = 5.f;
	bOrderExecuted = false;
	MaxPursuitDistance = 800;

	AggressiveType = EAIAggressiveType::Holding;
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);	
}

void ABaseAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if UE_EDITOR
	if(PerceptionComponent)
	{
		DrawDebugSphere(GetWorld(), GetPawn()->GetActorLocation(), SightConfig->SightRadius, 8, FColor::Blue, false, 0.f);
	}
#endif
}


void ABaseAIController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseAIController, TargetActor);
}

void ABaseAIController::SenseConfigInit()
{
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComponent")));

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("ConfigSight"));
	SightConfig->SightRadius = 1800.f;
	SightConfig->LoseSightRadius = SightConfig->SightRadius + 850.f;
	SightConfig->PeripheralVisionAngleDegrees = 360.f;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 100.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->SetMaxAge(2.f);

	//add sight configuration component to perception component
	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void ABaseAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);
	
	if(Result.Code == EPathFollowingResult::Success || Result.Code == EPathFollowingResult::Aborted)
	{
		bOrderExecuted = false;

		InitialOrderPoint = GetPawn()->GetActorLocation();
	}
}

void ABaseAIController::CheckHoldingDistance()
{
	if(FVector::Dist(GetPawn()->GetActorLocation(), InitialOrderPoint) >= MaxPursuitDistance)
	{
		TargetActor = nullptr;
		StopCheckDistanceForAttack();
		StopChasingTarget();

		if(GetAggressiveType() == EAIAggressiveType::Holding)
		{
			MoveToLocation(InitialOrderPoint, 15.f);
		}
		else StopMovement();
		
		GetWorld()->GetTimerManager().ClearTimer(CheckPursuitDistanceHandle);
	}
}

void ABaseAIController::MoveToGiveOrder(const FVector& Location, AActor* NewTargetActor)
{
	if(GetLocalRole() != ROLE_Authority) return;
	
	bOrderExecuted = true;
	TargetActor = NewTargetActor;
	InitialOrderPoint = GetPawn()->GetActorLocation();

	if(NewTargetActor)
	{
		MoveToActor(TargetActor, 25.f);
	}
	else
	{
		MoveToLocation(Location, 25.f);
	}
	ForceNetUpdate();
}

void ABaseAIController::ToggleAggressiveType()
{
	switch (AggressiveType)
	{
		case EAIAggressiveType::Holding:
		{
			AggressiveType = EAIAggressiveType::Pursuit;
			OnPursuitTypeActive();
			break;
		}
		case EAIAggressiveType::Pursuit:
		{
			AggressiveType = EAIAggressiveType::BuildDestroyed;
			break;
		}
		case EAIAggressiveType::BuildDestroyed:
		{
			AggressiveType = EAIAggressiveType::WaitCommand;
			break;
		}
		case EAIAggressiveType::WaitCommand:
		{
			AggressiveType = EAIAggressiveType::Attack;
			break;
		}	
		case EAIAggressiveType::Attack:
		{
			AggressiveType = EAIAggressiveType::Holding;
			OnHoldingTypeActive();
			break;
		}
		default: break;
	}
}

void ABaseAIController::OnHoldingTypeActive()
{
	MaxPursuitDistance = 800.f;
}

void ABaseAIController::OnPursuitTypeActive()
{
	MaxPursuitDistance = 1400.f;
}

void ABaseAIController::OnBuildDestroyedTypeActive()
{
	
}
