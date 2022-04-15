// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AISense_Sight.h"
#include "Perception/PawnSensingComponent.h"
#include "Enums/AIAggressiveTypeEnum.h"
#include "BaseAIController.generated.h"

UENUM(BlueprintType)
enum class EOrderType : uint8
{
	OrderExecuted,
	MoveToLocation,
	MoveToTarget
};

UCLASS()
class STRATEGYGAME_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

public:

	ABaseAIController();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual void MoveToGiveOrder(const FVector& Location, AActor* NewTargetActor);
	virtual void StopChasingTarget() {}

	EAIAggressiveType GetAggressiveType() const { return AggressiveType; }
	bool IsWaitCommandType() const { return AggressiveType == EAIAggressiveType::WaitCommand; }
	EOrderType GetOrderType() const { return OrderType; }
	FName GetFilterName() const { return FilterName; }

	UFUNCTION(BlueprintCallable)
	void ToggleAggressiveType();
	
protected:

	void SenseConfigInit();
	virtual void BeginPlay() override;
	virtual void OnSinglePerceptionUpdated(AActor* Actor, FAIStimulus  Stimulus) {}
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void StopCheckDistanceForAttack() {}
	virtual void CheckHoldingDistance();
	
	virtual void OnHoldingTypeActive();
	virtual void OnPursuitTypeActive();
	virtual void OnBuildDestroyedTypeActive();
	virtual void OnWaitCommandTypeActive();
	virtual void OnAttackTypeActive();

protected:

	UPROPERTY(BlueprintReadOnly, Replicated)
	AActor* TargetActor;
	
	UPROPERTY()
	EOrderType OrderType;
	
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	FTimerHandle LoseTargetHandle;

	//location before order for move to location
	UPROPERTY()
	FVector InitialOrderPoint;

	UPROPERTY()
	FTimerHandle CheckPursuitDistanceHandle;

	/** maximum target pursuit distance */
	UPROPERTY()
	float MaxPursuitDistance;

	UPROPERTY()
	FTimerHandle CheckDistanceForAttackHandle;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "AggressiveType", meta = (AllowPrivateAccess = "true"))
	EAIAggressiveType AggressiveType;

	/** the key name for filtering new targets that are included in perception. 
	* It is recorded when the type of AI behavior changes. */
	UPROPERTY()
	FName FilterName;
};
