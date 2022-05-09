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
	OrderType = EOrderType::OrderExecuted;
	MaxPursuitDistance = 800;
	FilterName = "";

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
	if(PerceptionComponent && GetLocalRole() == ROLE_Authority)
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
	SightConfig->LoseSightRadius = SightConfig->SightRadius;
	SightConfig->PeripheralVisionAngleDegrees = 360.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->SetMaxAge(0.1f);

	//add sight configuration component to perception component
	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->SetActive(true);
}

void ABaseAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if(OrderType == EOrderType::OrderExecuted) return;
	
	if(Result.Code == EPathFollowingResult::Success)
	{
		if(OrderType == EOrderType::MoveToLocation) OrderType = EOrderType::OrderExecuted;

		InitialOrderPoint = GetPawn()->GetActorLocation();
		FindNewTarget();
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
	
	TargetActor = NewTargetActor;
	InitialOrderPoint = GetPawn()->GetActorLocation();

	if(TargetActor)
	{
		MoveToActor(TargetActor, 25.f);
		OrderType = EOrderType::MoveToTarget;
	}
	else
	{
		MoveToLocation(Location, 25.f);
		OrderType = EOrderType::MoveToLocation;
	}
	ForceNetUpdate();
}
	
void ABaseAIController::ToggleAggressiveType(EAIAggressiveType Type)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("ToggleAggressiveType"));
	AggressiveType = Type;
	switch (AggressiveType)
	{
		case EAIAggressiveType::Holding:
		{
			OnHoldingTypeActive();
			break;
		}
		case EAIAggressiveType::Pursuit:
		{
			OnPursuitTypeActive();
			break;
		}
		case EAIAggressiveType::BuildDestroyed:
		{
			OnBuildDestroyedTypeActive();
			break;
		}
		case EAIAggressiveType::WaitCommand:
		{
			OnWaitCommandTypeActive();
			break;
		}	
		case EAIAggressiveType::Attack:
		{
			OnAttackTypeActive();
			break;
		}
		default: break;
	}
	Client_ToggleAggressiveTypeChanged();
}

void ABaseAIController::Server_ToggleAggressiveType_Implementation(EAIAggressiveType Type)
{
	ToggleAggressiveType(Type);	
}

void ABaseAIController::Client_ToggleAggressiveTypeChanged_Implementation()
{
	OnOrderTypeChanged.Broadcast();	
}

void ABaseAIController::OnHoldingTypeActive()
{
	MaxPursuitDistance = 800.f;
	FilterName = "";
}

void ABaseAIController::OnPursuitTypeActive()
{
	MaxPursuitDistance = 1400.f;
	FilterName = "";
}

void ABaseAIController::OnBuildDestroyedTypeActive()
{
	MaxPursuitDistance = 1400.f;
	FilterName = "Building";
}

void ABaseAIController::OnWaitCommandTypeActive()
{
	FilterName = "";
	StopChasingTarget();
	StopCheckDistanceForAttack();
	GetPerceptionComponent()->ToggleActive();
}

void ABaseAIController::OnAttackTypeActive()
{
	GetPerceptionComponent()->ToggleActive();
	MaxPursuitDistance = 0.f;
}

