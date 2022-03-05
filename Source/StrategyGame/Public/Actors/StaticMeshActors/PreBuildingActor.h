// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PreBuildingActor.generated.h"

class ASpectatorPlayerController;

UCLASS()
class STRATEGYGAME_API APreBuildingActor : public AActor
{
	GENERATED_BODY()

	UFUNCTION()
	void OnSpawnBuilding();
public:

	APreBuildingActor();
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	void SetBoxExtent(const FVector& Size) const { BoxCollision->SetBoxExtent(Size); }

	void SetOwnerController(ASpectatorPlayerController* Controller);
	void SetBuildingActor(TSubclassOf<ABaseBuildingActor> Class) { BuildingActorClass = Class; }
	void SetBuildRowName(const FName& NewName) { RowName = NewName; }

	UStaticMeshComponent* GetStaticMeshComponent() const { return MeshComponent; }
	UBoxComponent* GetBoxCollision() const { return BoxCollision; }

private:

	UPROPERTY(meta = (ExposeOfSpawn = "true"))
	ASpectatorPlayerController* OwnerPlayerController;

	UPROPERTY()
	UBoxComponent* BoxCollision;

	UPROPERTY()
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(meta = (ExposeOfSpawn = "true"))
	TSubclassOf<ABaseBuildingActor> BuildingActorClass;

	UPROPERTY(meta = (ExposeOfSpawn = "true"))
	FName RowName;

	/**
	 *  if true ignores one mouse click.
	 *  it is required so that the object does not create a real object after clicking on the slot
	 */
	bool bIgnoreSpawnPressed;
};
