// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Pawns/Base/BaseAggressivePawn.h"
#include "Net/UnrealNetwork.h"
#include "AI/Controllers/Base/BaseAIAggressiveController.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

ABaseAggressivePawn::ABaseAggressivePawn(const FObjectInitializer& ObjectInitializer)
{
	AIControllerClass = ABaseAIAggressiveController::StaticClass();
	bAttacking = false;

	SetAttackData(FAggressiveAttackData(780.f, 10.f, 1.f));
	
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

	/** if target can not have damage. m.b. Target is dead */
	if(!Target->CanBeDamaged())
	{
		StopAttack();
		ABaseAIAggressiveController* AggressiveController = Cast<ABaseAIAggressiveController>(Controller);
		if(AggressiveController)
		{
			AggressiveController->FindNewTarget();
		}
		return;
	}
	
	/** if target leave attack range stop attack and move to him */
	float const Dist = FVector::Dist(Target->GetActorLocation(), GetActorLocation());
	if(Dist >= AttackData.BaseDistanceForAttack)
	{
		StopAttack();
		ABaseAIAggressiveController* AggressiveController = Cast<ABaseAIAggressiveController>(Controller);
		if(AggressiveController)
		{
			AggressiveController->StartChasingTarget();
		}
		return;
	}
#if UE_EDITOR
	/** line trace for visible attack location */

	DrawDebugSphere(GetWorld(), GetActorLocation(), 15, 8.f, FColor::White, false, 1.f);
	DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Blue, false, 1.f);
	DrawDebugSphere(GetWorld(), Target->GetActorLocation(), 15, 8.f, FColor::Red, false, 1.f);
#endif
}

void ABaseAggressivePawn::StopAttack()
{
	GetWorld()->GetTimerManager().ClearTimer(RefreshAttackHandle);
	bAttacking = false;
}


