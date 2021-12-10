// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "BaseBuildingActor.generated.h"

UCLASS()
class STRATEGYGAME_API ABaseBuildingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseBuildingActor();

	UFUNCTION(BlueprintPure, Category = "Building|Getting")
	FText GetBuildingName() const { return BuildingName; }

	UFUNCTION(BlueprintPure, Category = "Building|Getting")
	FText GetDescription() const { return Description; }

	UFUNCTION(BlueprintPure, Category = "Building|Getting")
	UTexture2D* GetIcon() const;

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FText BuildingName;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FText Description;

	UPROPERTY(EditDefaultsOnly, Category = "Info")
	TAssetPtr<UTexture2D> Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta=(AllowPrivateAccess="true"))
	UBoxComponent* BoxCollision;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Info", meta=(AllowPrivateAccess="true"))
	UStaticMeshComponent* StaticMeshComponent;
};
