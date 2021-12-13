// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "PreBuildingActor.generated.h"

class ASpectatorPlayerController;

UCLASS()
class STRATEGYGAME_API APreBuildingActor : public AActor
{
	GENERATED_BODY()

public:

	APreBuildingActor();
	
	virtual void Tick(float DeltaSeconds) override;

	void SetOwnerController(ASpectatorPlayerController* Controller) { OwnerPlayerController = Controller; }

	UStaticMeshComponent* GetStaticMeshComponent() const { return MeshComponent; }
	UBoxComponent* GetBoxCollision() const { return BoxCollision; }

private:

	UPROPERTY()
	ASpectatorPlayerController* OwnerPlayerController;

	UPROPERTY()
	UBoxComponent* BoxCollision;

	UPROPERTY()
	UStaticMeshComponent* MeshComponent;
};
