// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/RepairInterface.h"
#include "ConstructionBuildingBase.generated.h"

UCLASS(Abstract)
class STRATEGYGAME_API AConstructionBuildingBase : public AActor, public IRepairInterface
{
	GENERATED_BODY()

	/** Start construction build
	 *
	 * @param Value Value to construction per second
	 */
	void StartConstruction(float const Value);

	UFUNCTION()
	void OnStopRegeneration();
	
public:	
	// Sets default values for this actor's properties
	AConstructionBuildingBase();

	class UObjectHealthComponent* GetHealthComponent() const { return ObjectHealthComponent; }

	bool IsBuildConstruction() const { return bBuildingConstruction; }
	class UObjectHealthComponent* GetObjectHealthComponent() const { return ObjectHealthComponent; }

	virtual void StartObjectRepair_Implementation(float const Value) override;
	virtual void StopObjectRepair_Implementation(float const Value) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void ConstructionSucceeded() {}

private:
    
	/** true if building be construction */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Replicated)
	bool bBuildingConstruction;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent", meta = (AllowPrivateAccess = "true"))
	class UObjectHealthComponent* ObjectHealthComponent;
};
