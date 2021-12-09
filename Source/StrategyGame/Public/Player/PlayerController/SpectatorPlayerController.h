// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpectatorPlayerController.generated.h"

class USpectatorCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetActorUpdated, AActor*, TargetActor);

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

	/** Call on select actor (left mouse button default) */
	void OnSelectActorPressed();

	/** Write new target actor */
	void SetTargetActor(AActor* NewTarget);

	/** Rep notify by target actor */
	UFUNCTION()
	void OnRep_TargetActor();

	/** Call on select actor on server if the client's checks were correct */
	UFUNCTION(Server, Reliable, Category = "Replicated")
	void Server_SelectedActor(AActor* SelectedActor);

protected:

	/** Input interface */
	virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;

	virtual void BeginPlay() override;

public:

	ASpectatorPlayerController(const FObjectInitializer& ObjectInitializer);
	
	virtual void UpdateRotation(float DeltaTime) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void SetupInputComponent() override;

private:

	UPROPERTY()
	UMaterialInstance* HighlightedMaterialInstance;
	
	UPROPERTY(ReplicatedUsing = OnRep_TargetActor)
	AActor* TargetActor;

public:

	UPROPERTY(BlueprintAssignable, Category = "Delegate", meta = (AllowPrivateAccess = "true"))
	FTargetActorUpdated TargetActorUpdated;
};
