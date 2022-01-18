// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "Player/Components/ResourcesActorComponent.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/FindObjectTeamInterface.h"

#include "SpectatorPlayerController.generated.h"

class USpectatorCameraComponent;
class AStrategyGameBaseHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetActorUpdated, AActor*, TargetActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionWithObjectReleased);

/** called when the player gives a command to the controlled pawn (right mouse button by default) */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionTargetActor);

UCLASS()
class STRATEGYGAME_API ASpectatorPlayerController : public APlayerController, public IFindObjectTeamInterface
{
	GENERATED_BODY()


	/** Helper for find spectator camera component from controlled pawn */
	USpectatorCameraComponent* GetSpectatorCameraComponent();

	/** Helper for get Strategy hud base */
	AStrategyGameBaseHUD* GetStrategyGameBaseHUD() const;

	/** Spawn post process on owning client for selection object */
	void SpawnPostProcess();

	void UpdateHighlightedActor();
	void DebugTraceFromMousePosition(const FHitResult& OutHit);

	/**
	 * On or Off custom depth on actor
	 *
	 * @param	Actor	Actor for update custom depth
	 * @param	bState	responsible for turning on or off custom depth on actor
	 */
	void UpdateCustomDepthFromActor(AActor* Actor, bool bState);

	/** Clear Target actor array and update custom depth */
	void ClearTargetActors();

	/** return vector2D with Mouse position  */
	FVector2D GetMousePositionCustom() const;

	/** Call on select actor (left mouse button default) */
	UFUNCTION()
	void OnSelectActorReleased();

	UFUNCTION()
	void OnActionWithObjectPressed();

	UFUNCTION()
	void OnActionTargetPawn();

	UFUNCTION()
	void MoveTargetPawnsPressed();

	UFUNCTION(Server, Unreliable)
	void Server_ActionTargetPawn();

	UFUNCTION(Server, Unreliable)
	void Server_MoveTargetPawns();

	UFUNCTION(Client, Unreliable)
	void Client_ForciblyDisablingSelectedObject();

	/*
	* Spawn building actor
	* 
	* @param	SpawnClass  Class for spawn
	* @param	Location	Location for spawn actor
	*/
	UFUNCTION(Server, Unreliable)
    void Server_SpawnBuilding(TSubclassOf<ABaseBuildingActor> SpawnClass, const FVector& Location);

protected:

	/** Input interface */
	virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;

	virtual void BeginPlay() override;
	virtual void OnRep_Pawn() override;
	virtual void OnPossess(APawn* InPawn) override;
public:

	ASpectatorPlayerController(const FObjectInitializer& ObjectInitializer);
	virtual void UpdateRotation(float DeltaTime) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void SetupInputComponent() override;
	virtual EObjectTeam FindObjectTeam_Implementation() override;
	const FHitResult& GetMousePositionResult() const { return MousePositionResult; }

	/**
	* Add resources value by resources type
	*
	* @param	Type	Type resources for add value
	* @param	Value	resources value for add to array	
	*/
	
	void AddResourcesByType(const EResourcesType Type, const int32 Value);

	/**
	* Decrease resources value by resources type
	*
	* @param	Type	Type resources for add value
	* @param	Value	resources value for add to array	
	*/
	void DecreaseResourcesByType(const EResourcesType Type, const int32 Value);

	/**
	 * start spawn building logic
	 *
	 * @param	BuildingClass	Class for spawn
	 */
	void SpawnBuilding(TSubclassOf<ABaseBuildingActor> BuildingClass);
	
	/**
	 *Add new target actor
	 *
	 * @param	NewTarget	new target for add to target
	 */
	void AddTargetActor(AActor* NewTarget);

	/**
	 * Add new targets array
	 *
	 * @param	NewTargets  Add new target array
	 */
	UFUNCTION(Server, Reliable)
	void Server_AddTargetActors(const TArray<AActor*>& NewTargets);

	UFUNCTION(Server, Reliable)
	void Server_SingleSelectActor(const FVector& TraceStart, const FVector& TraceEnd);

	UFUNCTION(Server, Reliable)
	void Server_ClearTargetActors();

	void ClearTargetAllActorsDepth();
	
private:

	UPROPERTY(VisibleDefaultsOnly, Category = "Controller|Components", meta=(AllowPrivateAcess = "true"))
	UResourcesActorComponent* ResourcesActorComponent;
	
	UPROPERTY()
	UMaterialInstance* HighlightedMaterialInstance;
	
	UPROPERTY(Replicated)
	TArray<AActor*> TargetActors;
	
	/** Mouse hit result with line trace from mouse position. Valid on owning client */
	FHitResult MousePositionResult;

public:

	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FTargetActorUpdated TargetActorUpdated;

	/** called when the player gives a command to the controlled pawn (right mouse button by default) */
	UPROPERTY()
	FActionTargetActor OnActionTargetActor;

	UPROPERTY()
	FActionWithObjectReleased OnActionWithObjectReleasedEvent;

	/** if true, line trace with mouse be ignore all components when can be selected. Live on client */
	bool bIgnoreHighlighted;
};
