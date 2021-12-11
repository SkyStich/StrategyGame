// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs/SelectedObjectInfoBase.h"
#include "Player/Interfaces/HighlightedInterface.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "BaseBuildingActor.generated.h"

UCLASS()
class STRATEGYGAME_API ABaseBuildingActor : public AActor, public IHighlightedInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBuildingActor();

	UFUNCTION(BlueprintPure, Category = "Building|Getting")
	FName GetRowName() const { return RowName; }

	UFUNCTION(BlueprintPure, Category = "Building|Getting")
	FSelectedObjectInfo GetSelectedObjectInfo() const { return *SelectedObjectInfo; }
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces", meta=(DisplayName="BP_HilighlightInterface"))
    void HighlightedActor(APlayerController* PlayerController);

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FName RowName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta=(AllowPrivateAccess="true"))
	UBoxComponent* BoxCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* StaticMeshComponent;

	FSelectedObjectInfo* SelectedObjectInfo;
};
