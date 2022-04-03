// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Pawns/Base/BaseAIPawn.h"
#include "BaseBuilderPawn.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API ABaseBuilderPawn : public ABaseAIPawn
{
	GENERATED_BODY()

	UFUNCTION()
	void CheckDistanceToBuilding(AActor* ActorToMove);
	
	UFUNCTION()
	void OnRepairFinish();

public:

	virtual void HighlightedActor_Implementation(AStrategyGameBaseHUD* PlayerHUD) override;
	virtual bool GiveOrderToTargetPawn_Implementation(const FVector& LocationToMove, AActor* ActorToMove) override;

private:

	UPROPERTY()
	FTimerHandle CheckDistanceToBuildingHandle;

	//Valid only on authority
	UPROPERTY()
	AActor* CurrentRepairActor;
};
