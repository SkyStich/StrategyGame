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
class AStrategyMatchPlayerState;

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

	void RemoveGroupSelection(AStrategyGameBaseHUD* StrategyHUD);
	void BindOnTargetActorDeath(AActor* Target);
	void UnBindTargetActorDeath(AActor* Target);

	/** helper for find spectator player state */
	AStrategyMatchPlayerState* GetStrategyPlayerState() const;

	/** Helper for get Strategy hud base */
	AStrategyGameBaseHUD* GetStrategyGameBaseHUD() const;

	/** Spawn post process on owning client for selection object */
	void SpawnPostProcess();

	/** update highlight. call on tick */
	void UpdateHighlightedActor();

	/** Clear all Target actor array */
	void ClearTargetActors();
	
	/** remove single actor from array */
	void RemoveActorFromTarget(AActor* Actor);

	/** return vector2D with Mouse position  */
	FVector2D GetMousePositionCustom() const;

	/** Call on select actor (left mouse button default) */
	UFUNCTION()
	void OnSelectActorReleased();
	
	UFUNCTION()
	void OnTargetDeath(AActor* Actor);

	UFUNCTION()
	void OnActionWithObjectPressed();

	UFUNCTION()
	void OnActionTargetPawn();

	UFUNCTION()
	void MoveTargetPawnsPressed();

	UFUNCTION(Server, Unreliable)
	void Server_ActionTargetPawn();

	UFUNCTION(Server, Reliable)
	void Server_MoveTargetPawns(const FVector& TraceStart, const FVector& TraceEnd);

	UFUNCTION(Client, Unreliable)
	void Client_CallHighlightedOnSelectObject(AActor* Target);

	/*
	* Spawn building actor
	* 
	* @param	SpawnClass  Class for spawn
	* @param	Location	Location for spawn actor
	* @param	BuildName	Build display name for clients
	*/
	UFUNCTION(Server, Reliable)
    void Server_SpawnBuilding(TSubclassOf<ABaseBuildingActor> SpawnClass, const FVector& Location, const FText& BuildName);

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

	/** return line trace result with current mouse position */
	const FHitResult& GetMousePositionResult() const { return MousePositionResult; }

	/**
	* On or Off custom depth on actor
	*
	* @param	Actor	Actor for update custom depth
	* @param	bState	responsible for turning on or off custom depth on actor
	*/
	void UpdateCustomDepthFromActor(AActor* Actor, bool bState);

	/**
	 * Spawn pre building actor for find spawn build location
	 * @param BuildActor Build class for spawn 
	 */
	void SpawnPreBuildAction(TSubclassOf<ABaseBuildingActor> BuildActor);

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
	 * @param	BuildName	Build display name for clients
	 */
	void SpawnBuilding(TSubclassOf<ABaseBuildingActor> BuildingClass, const FText& BuildName);
	
	/**
	 *Add new single target actor in array
	 *
	 * @param	NewTarget	new target for add to target
	 */
	void AddSingleTargetActor(AActor* NewTarget);

	/**
	 * Add new targets pawn with group selecting
	 *
	 * @param	NewTargets  Add new target array
	 */
	UFUNCTION(Server, Reliable)
	void Server_AddTargetPawns(const TArray<class ABaseAIPawn*>& NewTargets);

	/**
	* update client logic for add target pawns with group selection and call server notify for add target actors
	*
	* @param	NewTargets  Add new target array
	*/
	void AddTargetPawns(const TArray<class ABaseAIPawn*>& NewTargets);

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

	FName HighlightedTag;
};
