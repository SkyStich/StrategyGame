// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Interfaces/HighlightedInterface.h"
#include "Components/BoxComponent.h"
#include "Net/RepLayout.h"
#include "GameFramework/Actor.h"
#include "Interfaces/GiveOrderToTargetPawns.h"
#include "Enums/TeamData.h"
#include "Interfaces/FindObjectTeamInterface.h"
#include "Player/UI/MathWidgetBase/BaseMatchWidget.h"
#include "Structs/AllianceBuildingStructures.h"
#include "Structs/AllianceAIData.h"
#include "Interfaces/ImprovementInterface.h"
#include "Structs/ImprovementData.h"
#include "BaseBuildingActor.generated.h"

USTRUCT(BlueprintType)
struct FQueueData
{
	GENERATED_BODY()

	FQueueData() : QueueId(""), Value(FAIPawnData()) {}
	FQueueData(const FName& Key, const FAIPawnData& Data) : QueueId(Key), Value(Data) {}

	UPROPERTY(BlueprintReadOnly)
	FName QueueId;

	UPROPERTY(BlueprintReadOnly)
	FAIPawnData Value;
};

UCLASS(Abstract, Blueprintable)
class STRATEGYGAME_API ABaseBuildingActor : public AActor, public IHighlightedInterface,
	public IGiveOrderToTargetPawns, public IFindObjectTeamInterface, public IFindObjectNameInterface, public IImprovementInterface
{
	GENERATED_BODY()

	/** Start spawn pawn client logic */
	UFUNCTION()
	void StartSpawnTimer(const FName& Key);

	UFUNCTION()
	void OnSpawnComplete();

	UFUNCTION()
	void RefreshSpawnTimer();
	/** End spawn pawn client logic */
	
	void StartSpawnPawn(const FName& Key);

	UFUNCTION(Server, Unreliable)
	void Server_SpawnPawn(const FName& Key);

	UFUNCTION(Server, Reliable)
	void Server_BuildImprovement(const FName& RowName);
	
	UFUNCTION(Server, Unreliable)
	void Server_Highlighted();

	UFUNCTION(Server, Unreliable)
	void Server_UnHighlighted();

	/** remove object from queue
	 *
	 * @param QueueId Id in queue (not row name in data table)
	 */
	UFUNCTION(Server, Unreliable)
	void Server_RemoveItemFromQueue(const FName& QueueId);

	UFUNCTION(Client, Unreliable)
	void Client_OnSpawnFinished(const FName& Key);

	UFUNCTION()
	void OnSpawnPawn(FQueueData QueueData, FVector SpawnLocation);

	UFUNCTION()
	void UnHighlighted();

	UFUNCTION()
	void OnBuildingImprovementCompleted(FImprovementQueue ImprovementData);

	UFUNCTION()
	void StartImprovement(const FImprovementQueue& ImprovementData);

	void RefreshQueue();
	FVector FindSpawnLocation();
	
public:	
	// Sets default values for this actor's properties
	ABaseBuildingActor();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	virtual void GiveOrderToTargetPawn_Implementation(const FVector& LocationToMove, AActor* ActorToMove) override;
	virtual EObjectTeam FindObjectTeam_Implementation() override;
	virtual void HighlightedActor_Implementation(AStrategyGameBaseHUD* PlayerHUD) override;
	virtual FText FindObjectName_Implementation() const override { return BuildName; }
	virtual bool ObjectImprovement_Implementation(const FName& RowName) override;

	FTimerHandle GetProgressTimerHandle() const { return ProgressTimerHandle; }

	void RemoveSlotFromQueue(USpawnProgressSlotBase* SlotForRemove);
	void SpawnPawn(const FName& Id);
	void SetOwnerController(class ASpectatorPlayerController* Controller);
	void SetTeamOwner(EObjectTeam Team) { OwnerTeam = Team; }
	
	class ASpectatorPlayerController* GetOwnerController() const { return OwnerPlayerController; }

	/*  set build name
	 *
	 * @param Name Display name this building
	 **/
	void SetBuildName(const FText& Name) { BuildName = Name; }

	UFUNCTION(Server, Unreliable)
	void Server_ClearSpawnPawnHandle(const FName& Key);

	/** generate queue slot from new item in queue */
	UFUNCTION(BlueprintCallable)
    void GenerateQueueSlot(const FName& Id);

protected:

	virtual void BeginPlay() override;

	/** generate queue slots from current queue */
	UFUNCTION(BlueprintCallable)
	void GenerateQueueSlots();

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta=(AllowPrivateAccess="true"))
	UBoxComponent* BoxCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Owners|PlayerController", meta=(AllowPrivateAccess="true", ExposeOfSpawn = true))
	class ASpectatorPlayerController* OwnerPlayerController;

	/** The point to which pawns will aim after emerging from this building  */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Owners|PlayerController", meta=(AllowPrivateAccess="true"))
	FVector InitialDestination;

	/** Spawn pawn row names from table DT_AIPawnData */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnData", meta = (AllowPrivateAccess = "true"))
	TArray<FName> PawnRowNames;

	UPROPERTY(Replicated)
	TArray<FQueueData> QueueOfSpawn;

	UPROPERTY(Replicated)
	TArray<FImprovementQueue> ImprovementQueue; 

	UPROPERTY(Replicated, meta = (ExposeOfSpawn = "true"))
	FText BuildName;
	
	UPROPERTY(Replicated, meta = (ExposeOfSpawn = "true"))
	EObjectTeam OwnerTeam;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTimerHandle ProgressTimerHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Improvement", meta = (AllowPrivateAccess = "true"))
	TArray<FImprovementLevelInfo> ImprovementLevelInfo;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "HealthComponent", meta = (AllowPrivateAccess = "true"))
	UObjectHealthComponent* ObjectHealthComponent;

	/** true if build is highlighted */
	bool bIsHighlighted;
};
