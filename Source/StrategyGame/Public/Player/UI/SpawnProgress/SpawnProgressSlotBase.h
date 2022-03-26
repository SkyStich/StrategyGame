// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/ProgressSlotInterface.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "Blueprint/UserWidget.h"
#include "SpawnProgressSlotBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSlotRemoved, USpawnProgressSlotBase*, RemoveSlot);

UCLASS()
class STRATEGYGAME_API USpawnProgressSlotBase : public UUserWidget, public IProgressSlotInterface
{
	GENERATED_BODY()

public:

	void SetSpawnTime(const float NewTime) { Time = NewTime; }
	void SetIcon(UTexture2D* Texture) { Icon = Texture; }
	void SetId(const FName& NewId) { Id = NewId; }
	FName GetId() const { return Id; }

	UFUNCTION(BlueprintCallable)
	virtual void RemoveSlotFromQueue();

	virtual void RemoveFromParent() override;
	
protected:

	UPROPERTY(BlueprintReadOnly)
	float Time;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* Icon;
	
	UPROPERTY(BlueprintReadOnly)
	FName Id;

public:

	UPROPERTY()
	FSlotRemoved OnSlotRemoved;
};
