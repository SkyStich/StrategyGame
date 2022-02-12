// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Interfaces/HighlightedInterface.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Interfaces/GiveOrderToTargetPawns.h"
#include "NavigationSystem.h"
#include "Enums/TeamData.h"
#include "Interfaces/FindObjectTeamInterface.h"
#include "Player/UI/MathWidgetBase/BaseMatchWidget.h"
#include "Structs/AllianceBuildingStructures.h"
#include "Structs/AllianceAIData.h"
#include "BaseBuildingActor.generated.h"

class ASpectatorPlayerController;

USTRUCT(BlueprintType)
struct FQueueData
{
	GENERATED_BODY()

	FQueueData() : RowName(""), PawnClass(nullptr), TimeBeforeSpawn(1.f), Icon(nullptr), ResourcesNeedToBuy() {}
	FQueueData(const FName& Key, float Time, TAssetPtr<UTexture2D> Texture, TAssetSubclassOf<ABaseAIPawn> Class, TArray<FResourcesData> ResourcesData) : 
	RowName(Key), PawnClass(Class), TimeBeforeSpawn(Time), Icon(Texture), ResourcesNeedToBuy(ResourcesData) {}

	UPROPERTY(BlueprintReadOnly)
	FName RowName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TAssetSubclassOf<ABaseAIPawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TimeBeforeSpawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TAssetPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FResourcesData> ResourcesNeedToBuy;
};

UCLASS(Abstract, Blueprintable)
class STRATEGYGAME_API ABaseBuildingActor : public AActor, public IHighlightedInterface, public IGiveOrderToTargetPawns, public IFindObjectTeamInterface
{
	GENERATED_BODY()
	
	void StartSpawnPawn(const FName& Key);

	UFUNCTION(Server, Unreliable)
	void Server_SpawnPawn(const FName& Key);

	UFUNCTION(Server, Unreliable)
	void Server_RemoveItemFromQueue(const FName& Id);

	UFUNCTION(Client, Unreliable)
	void Client_OnSpawnFinished(const FName& Key);

	UFUNCTION()
	void OnSpawnPawn(FName Key, FVector SpawnLocation, TAssetSubclassOf<class ABaseAIPawn> SpawnClass);

	UFUNCTION()
	void UnHighlighted();

	void RefreshQueue();
	FVector FindSpawnLocation();
	
public:	
	// Sets default values for this actor's properties
	ABaseBuildingActor();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void GiveOrderToTargetPawn_Implementation(const FVector& LocationToMove, AActor* ActorToMove) override;
	virtual EObjectTeam FindObjectTeam_Implementation() override;
	virtual void HighlightedActor_Implementation(AStrategyGameBaseHUD* PlayerHUD) override;

	void RemoveSlotFromQueue(USpawnProgressSlotBase* SlotForRemove);
	void SpawnPawn(const FName& Id);
	void SetOwnerController(ASpectatorPlayerController* Controller);
	void SetTeamOwner(EObjectTeam Team) { OwnerTeam = Team; }

	UFUNCTION(Server, Unreliable)
	void Server_ClearSpawnPawnHandle(const FName& Key);

	/** generate queue slot from new item in queue */
	UFUNCTION(BlueprintCallable)
    void GenerateQueueSlot(const FName& Id);

protected:

	virtual void BeginPlay() override;
	ASpectatorPlayerController* GetOwnerController() const { return OwnerPlayerController; }

	/** generate queue slots from current queue */
	UFUNCTION(BlueprintCallable)
	void GenerateQueueSlots();

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta=(AllowPrivateAccess="true"))
	UBoxComponent* BoxCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Owners|PlayerController", meta=(AllowPrivateAccess="true"))
	ASpectatorPlayerController* OwnerPlayerController;

	/** The point to which pawns will aim after emerging from this building  */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Owners|PlayerController", meta=(AllowPrivateAccess="true"))
	FVector InitialDestination;

	/** Spawn pawn row names from table DT_AIPawnData */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnData", meta = (AllowPrivateAccess = "true"))
	TArray<FName> PawnRowNames;

	UPROPERTY()
	TSubclassOf<class USpawnProgressSlotBase> SpawnProgressSlot;

	UPROPERTY(Replicated)
	TArray<FQueueData> QueueOfSpawn;
	
	UPROPERTY(Replicated)
	EObjectTeam OwnerTeam;
	
	UPROPERTY()
	FTimerHandle SpawnPawnHandle;

	/** true if build is highlighted */
	bool bIsHighlighted;
};
