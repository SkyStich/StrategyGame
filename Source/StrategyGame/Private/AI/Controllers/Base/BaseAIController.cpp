// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Controllers/Base/BaseAIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Net/UnrealNetwork.h"

ABaseAIController::ABaseAIController()
{
	bAttachToPawn = true;
	bReplicates = true;
	NetUpdateFrequency = 1.f;
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();
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
	SightConfig->SightRadius = 1000.f;
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
	
	if(Result.Code == EPathFollowingResult::Success)
	{
		bOrderExecuted = false;
	}
}


void ABaseAIController::MoveToGiveOrder(const FVector& Location, AActor* NewTargetActor)
{
	if(GetLocalRole() != ROLE_Authority) return;
	
	bOrderExecuted = true;
	TargetActor = NewTargetActor;
	MoveToLocation(Location, 15.f);
}