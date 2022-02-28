// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Pawns/Base/BaseAIPawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "BaseAggressivePawn.generated.h"

UCLASS(Abstract)
class STRATEGYGAME_API ABaseAggressivePawn : public ABaseAIPawn
{
	GENERATED_BODY()

protected:

	virtual void AttackTargetActor(AActor* Target);

public:
	ABaseAggressivePawn(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void StartAttackTargetActor(AActor* Target);
	virtual void StopAttack() override;

private:

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "SkeletalMesh|WeaponMesh", meta=(AllowPrivateAccess="true"))
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(Replicated)
	bool bAttacking;

	UPROPERTY()
	FTimerHandle RefreshAttackHandle;
};
