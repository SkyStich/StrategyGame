// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/Base/BaseAIController.h"

#include "Interfaces/FindObjectTeamInterface.h"
#include "Net/UnrealNetwork.h"

ABaseAIController::ABaseAIController()
{
	bReplicates = true;
	NetUpdateFrequency = 1.f;
	
	bAttachToPawn = true;

	LookPawnRadius = CreateDefaultSubobject<USphereComponent>(TEXT("LookPawnRadiuse"));
	LookPawnRadius->SetupAttachment(RootComponent);
	LookPawnRadius->InitSphereRadius(680);
	LookPawnRadius->SetCollisionResponseToAllChannels(ECR_Ignore);
	LookPawnRadius->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	LookPawnRadius->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	LookPawnRadius->SetHiddenInGame(false);
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() == ROLE_Authority)
	{
		LookPawnRadius->OnComponentBeginOverlap.AddDynamic(this, &ABaseAIController::OnComponentBeginOverlap);
		LookPawnRadius->OnComponentEndOverlap.AddDynamic(this, &ABaseAIController::OnComponentEndOverlap);
	}
}

void ABaseAIController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseAIController, TargetActor);
}


void ABaseAIController::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ABaseAIController::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}
