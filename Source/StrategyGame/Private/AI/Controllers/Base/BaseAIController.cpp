// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/Controllers/Base/BaseAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Net/UnrealNetwork.h"

ABaseAIController::ABaseAIController()
{
	bAttachToPawn = true;
	bReplicates = true;
	NetUpdateFrequency = 1.f;

	SenseConfigInit();
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
	SightConfig->SightRadius = 2500.f;
	SightConfig->LoseSightRadius = SightConfig->SightRadius + 850.f;
	SightConfig->PeripheralVisionAngleDegrees = 85.f;
	SightConfig->AutoSuccessRangeFromLastSeenLocation = 5.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->SetMaxAge(5.f);

	//add sight configuration component to perception component
	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseAIController::OnSinglePerceptionUpdated);
}

void ABaseAIController::OnSinglePerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
		
}

void ABaseAIController::MoveToGiveOrder(const FVector& Location, AActor* NewTargetActor)
{
	if(NewTargetActor)
	{
		TargetActor = NewTargetActor;
		MoveToActor(NewTargetActor, 15.f);
		return;
	}
	MoveToLocation(Location, 15.f);
}
