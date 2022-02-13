// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "Blueprint/UserWidget.h"
#include "SpawnProgressSlotBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSlotRemoved, USpawnProgressSlotBase*, RemoveSlot);

UCLASS()
class STRATEGYGAME_API USpawnProgressSlotBase : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetSpawnTime(const float Time) { SpawnTime = Time; }
	void SetIcon(UTexture2D* Texture) { Icon = Texture; }
	void SetId(const FName& NewId) { Id = NewId; }
	void SetBuildOwner(ABaseBuildingActor* NewOwner) { BuildOwner = NewOwner; }
	FName GetId() const { return Id; }

	UFUNCTION(BlueprintCallable)
	void RemoveSlotFromQueue();

	virtual void RemoveFromParent() override;
	
protected:

	UPROPERTY(BlueprintReadOnly)
	float SpawnTime;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* Icon;
	
	UPROPERTY(BlueprintReadOnly)
	FName Id;

	UPROPERTY(BlueprintReadOnly)
	ABaseBuildingActor* BuildOwner;

public:

	UPROPERTY()
	FSlotRemoved OnSlotRemoved;
};
