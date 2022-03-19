// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PlayerImprovementCharacteristics.generated.h"

class ASpectatorPlayerController;

UCLASS(Abstract)
class STRATEGYGAME_API UPlayerImprovementCharacteristics : public UObject
{
	GENERATED_BODY()

public:

	UPlayerImprovementCharacteristics(const FObjectInitializer& ObjectInitializer);
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual bool CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack) override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual void PostInitProperties() override;
	void SetOwner(ASpectatorPlayerController* NewOwner) { Owner = NewOwner; }

	virtual void ImprovementObjectComplete(class ABaseBuildingActor* BuildingForImprovement) { NativeImprovementObjectComplete(BuildingForImprovement); }	
protected:

	void BeginPlay();

	UFUNCTION(BlueprintPure)
	bool IsAuthority() const;
	
	UFUNCTION(BlueprintImplementableEvent)
	void NativeImprovementObjectComplete(class ABaseBuildingActor* BuildingForImprovement);

private:

	UPROPERTY(BlueprintReadOnly, Category = "Owner", meta = (AllowPrivateAccess = "true"))
	ASpectatorPlayerController* Owner;
};
