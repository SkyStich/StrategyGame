// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Pawns/Base/BaseAggressivePawn.h"

#include "AI/Controllers/Base/BaseAIAggressiveController.h"

ABaseAggressivePawn::ABaseAggressivePawn(const FObjectInitializer& ObjectInitializer)
{
	AIControllerClass = ABaseAIAggressiveController::StaticClass();
	
	WeaponMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponSkeletalMesh"));
	WeaponMesh->SetupAttachment(SkeletalMesh);
}

void ABaseAggressivePawn::AttackTargetActor(AActor* Target)
{
	
}
