// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "Enums/ResourcesData.h"
#include "ResourceProducingBuildingsBase.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API AResourceProducingBuildingsBase : public ABaseBuildingActor
{
	GENERATED_BODY()

private:

	void IncreaseResourcesTime(const float Value);
	void DecreaseResourcesTime(const float Value);

	void SetResourcesTimer();
	void ClearResourcesTimer();

	UFUNCTION()
	void UpdateResources();

public:

	AResourceProducingBuildingsBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
	virtual void ConstructionSucceeded() override;

private:
	
	/** Resources which be updated */
	UPROPERTY(EditDefaultsOnly, Category = "Resources|ResourcesData", meta = (AllowPrivateAccess))
	TMap<EResourcesType, int32> IncreaseResourcesData;

	/** The period with which the player's resources will be updated (in sec) */
	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Resources|ResourcesData", meta = (AllowPrivateAccess))
	int32 UpdateResourcesTime;

	UPROPERTY()
	FTimerHandle UpdateResourceHandle;
};
