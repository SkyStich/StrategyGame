// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Pawns/Base/BaseAggressivePawn.h"
#include "Net/UnrealNetwork.h"
#include "AI/Controllers/Base/BaseAIAggressiveController.h"
#include "Kismet/GameplayStatics.h"

ABaseAggressivePawn::ABaseAggressivePawn(const FObjectInitializer& ObjectInitializer)
{
	AIControllerClass = ABaseAIAggressiveController::StaticClass();
	bAttacking = false;

	SetAttackData(FAggressiveAttackData(280.f, 10.f, 1.f));
	
	WeaponMesh = ObjectInitializer.CreateDefaultSubobject<USkeletalMeshComponent>(this, TEXT("WeaponSkeletalMesh"));
	WeaponMesh->SetupAttachment(GetMesh(), "SKT_WeaponPoint");
}

void ABaseAggressivePawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseAggressivePawn, bAttacking);
}

void ABaseAggressivePawn::StartAttackTargetActor(AActor* Target)
{
	if(GetLocalRole() != ROLE_Authority || bAttacking || !Target) return;

	bAttacking = true;
	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &ABaseAggressivePawn::AttackTargetActor, Target);
	GetWorld()->GetTimerManager().SetTimer(RefreshAttackHandle, TimerDel, GetAggressiveAttackData()->BaseDelayBeforeUsed, true);
}

void ABaseAggressivePawn::AttackTargetActor(AActor* Target)
{
	if(!Target)
	{
		StopAttack();
		return;
	}
	UGameplayStatics::ApplyDamage(Target, GetAggressiveAttackData()->BaseDamage, GetController(), this, UDamageType::StaticClass());
}

void ABaseAggressivePawn::StopAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(RefreshAttackHandle);
	bAttacking = false;
}


