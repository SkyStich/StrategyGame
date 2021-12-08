// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpectatorPlayerController.generated.h"

class USpectatorCameraComponent;

UCLASS()
class STRATEGYGAME_API ASpectatorPlayerController : public APlayerController
{
	GENERATED_BODY()

	/** Helper for find spectator camera component from controlled pawn */
	USpectatorCameraComponent* GetSpectatorCameraComponent();

	/** Spawn post process on owning client for selection object */
	void SpawnPostProcess();

	void UpdateHighlightedActor();
	void UpdateCustomDepthFromActor(AActor* Actor, bool bState);
	void DebugTraceFromMousePosition(const FHitResult& OutHit);

protected:

	/** Input interface */
	virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;
	
	virtual void BeginPlay() override;

public:

	ASpectatorPlayerController(const FObjectInitializer& ObjectInitializer);
	
	virtual void UpdateRotation(float DeltaTime) override;
	virtual void Tick(float DeltaSeconds) override;

private:

	UPROPERTY()
	UMaterialInstance* HighlightedMaterialInstance;
};
