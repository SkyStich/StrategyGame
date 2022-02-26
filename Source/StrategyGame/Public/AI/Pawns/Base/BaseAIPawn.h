// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/TeamData.h"
#include "GameFramework/Character.h"
#include "Interfaces/FindObjectNameInterface.h"
#include "Interfaces/FindObjectTeamInterface.h"
#include "Interfaces/GiveOrderToTargetPawns.h"
#include "Player/Interfaces/HighlightedInterface.h"
#include "Structs/AllianceAIData.h"

#include "BaseAIPawn.generated.h"

class UObjectHealthComponent;

USTRUCT(BlueprintType)
struct FAggressiveAttackData
{
	GENERATED_BODY()

	FAggressiveAttackData() : BaseDistanceForAttack(0.f), BaseDamage(0.f), BaseDelayBeforeUsed(0.f) {}
	FAggressiveAttackData(float Distance, float Damage, float DelayBeforeUsed) :
    BaseDistanceForAttack(Distance), BaseDamage(Damage), BaseDelayBeforeUsed(DelayBeforeUsed) {}

	UPROPERTY(BlueprintReadOnly)
	float BaseDistanceForAttack;

	UPROPERTY(BlueprintReadOnly)
	float BaseDamage;
	
	UPROPERTY(BlueprintReadOnly)
	float BaseDelayBeforeUsed;
};

UCLASS(Abstract)
class STRATEGYGAME_API ABaseAIPawn : public ACharacter, public IHighlightedInterface, public IGiveOrderToTargetPawns, public IFindObjectTeamInterface, public IFindObjectNameInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseAIPawn();
	
	virtual void GiveOrderToTargetPawn_Implementation(const FVector& LocationToMove, AActor* ActorToMove) override;
	virtual FText FindObjectName_Implementation() const override { return PawnName; }
	void InitPawn(const FAIPawnData& PawnData);
	void SetTeam(EObjectTeam Team)  { OwnerTeam = Team; }
	void SetAttackData(const FAggressiveAttackData& Data) { AttackData = Data; }
	EObjectTeam GetTeam() const { return OwnerTeam; }
	FAggressiveAttackData* GetAggressiveAttackData() { return &AttackData; }

	/** start functions for initialize base pawn params */
	
	/**
	* Set current health value by default. Call on created object
	* 
	* @param Value Value for set new health
	*/
	void SetHealthDefault(float const Value);
	
	/** stop functions for initialize base pawn params */

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual EObjectTeam FindObjectTeam_Implementation() override { return OwnerTeam; }

protected:

	UPROPERTY(Replicated, EditAnyWhere)
	EObjectTeam OwnerTeam;

	UPROPERTY(BlueprintReadOnly)
	UObjectHealthComponent* ObjectHealthComponent;

private:

	UPROPERTY()
	FAggressiveAttackData AttackData;
	
	UPROPERTY(Replicated)
	FName PawnRowName;
	
	UPROPERTY(Replicated)
	FText PawnName;
};
