// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Interfaces/HighlightedInterface.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "Player/Components/ResourcesActorComponent.h"

#include "BaseBuildingActor.generated.h"

class ASpectatorPlayerController;

UCLASS(Abstract, Blueprintable)
class STRATEGYGAME_API ABaseBuildingActor : public AActor, public IHighlightedInterface
{
	GENERATED_BODY()

	UFUNCTION(Server, Unreliable)
	void Server_SpawnPawn(TSubclassOf<class ABaseAIPawn> PawnClass, const TArray<FResourcesData>& ResourcesNeedToBuy);
	
public:	
	// Sets default values for this actor's properties
	ABaseBuildingActor();

	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	void SpawnPawn(TSubclassOf<class ABaseAIPawn> PawnClass, const TArray<FResourcesData>& ResourcesNeedToBuy);
	void SetOwnerController(ASpectatorPlayerController* Controller);

	UFUNCTION(BlueprintPure, Category = "Building|Getting")
	FName GetRowName() const { return RowName; }

protected:

	virtual void BeginPlay() override;
	ASpectatorPlayerController* GetOwnerController() const { return OwnerPlayerController; }

private:

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FName RowName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta=(AllowPrivateAccess="true"))
	UBoxComponent* BoxCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Owners|PlayerController", meta=(AllowPrivateAccess="true"))
	ASpectatorPlayerController* OwnerPlayerController;
};
