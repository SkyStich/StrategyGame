// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Interfaces/HighlightedInterface.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "BaseBuildingActor.generated.h"

class ASpectatorPlayerController;

UCLASS(Abstract, Blueprintable)
class STRATEGYGAME_API ABaseBuildingActor : public AActor, public IHighlightedInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBuildingActor();

	/** IHighlightedInterface */
	virtual void HighlightedActor_Implementation(APlayerController* PlayerController) override;

	virtual void Tick(float DeltaTime) override;

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

	UPROPERTY(BlueprintReadOnly, Category = "Owners|PlayerController", meta=(AllowPrivateAccess="true"))
	ASpectatorPlayerController* OwnerPlayerController;

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = Info)
	TAssetSubclassOf<UUserWidget> ActionObjectGrid;
};
