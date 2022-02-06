// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Enums/TeamData.h"
#include "Components/SphereComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/PawnSensingComponent.h"

#include "BaseAIController.generated.h"

UCLASS()
class STRATEGYGAME_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

	void SenseConfigInit();

public:

	ABaseAIController();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void MoveToGiveOrder(const FVector& Location, AActor* NewTargetActor);
	
protected:

	virtual void BeginPlay() override;
	virtual void OnSinglePerceptionUpdated(AActor* Actor, FAIStimulus  Stimulus);

protected:

	UPROPERTY(BlueprintReadOnly, Replicated)
	AActor* TargetActor;
	
private:

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UAISenseConfig_Sight* SightConfig;
};
