// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpectatorPlayerController.generated.h"

class USpectatorCameraComponent;
class AStrategyGameBaseHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetActorUpdated, AActor*, TargetActor);

UCLASS()
class STRATEGYGAME_API ASpectatorPlayerController : public APlayerController
{
	GENERATED_BODY()

	/** Helper for find spectator camera component from controlled pawn */
	USpectatorCameraComponent* GetSpectatorCameraComponent();

	/** Helper for get Strategy hud base */
	AStrategyGameBaseHUD* GetStrategyGameBaseHUD() const;

	/** Spawn post process on owning client for selection object */
	void SpawnPostProcess();

	void UpdateHighlightedActor();
	void UpdateCustomDepthFromActor(AActor* Actor, bool bState);
	void DebugTraceFromMousePosition(const FHitResult& OutHit);

	/** Call on select actor (left mouse button default) */
	void OnSelectActorPressed();

	/** Add new target actor */
	void AddTargetActor(AActor* NewTarget);
	void ClearTargetActors();

protected:

	/** Input interface */
	virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;

	virtual void BeginPlay() override;
	virtual void OnRep_Pawn() override;

public:

	ASpectatorPlayerController(const FObjectInitializer& ObjectInitializer);
	
	virtual void UpdateRotation(float DeltaTime) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void SetupInputComponent() override;

	const FHitResult& GetMousePositionResult() const { return MousePositionResult; }

private:

	UPROPERTY()
	UMaterialInstance* HighlightedMaterialInstance;
	
	UPROPERTY()
	TArray<AActor*> TargetActors;
	
	/** Mouse hit result with line trace from mouse position. Valid on owning client */
	FHitResult MousePositionResult;

public:

	UPROPERTY(BlueprintAssignable, Category = "Delegate", meta = (AllowPrivateAccess = "true"))
	FTargetActorUpdated TargetActorUpdated;

	/** if true, line trace with mouse be ignore all components when can be selected. Live on client */
	bool bIgnoreHighlighted;
};
