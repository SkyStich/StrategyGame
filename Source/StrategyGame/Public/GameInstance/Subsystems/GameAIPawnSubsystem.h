// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Enums/TeamData.h"
#include "Engine/DataTable.h"
#include "GameAIPawnSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API UGameAIPawnSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	UGameAIPawnSubsystem();

	UDataTable* GetPawnDataByTeam(EObjectTeam OwnerTeam);
	UDataTable* GetBuilderDataByTeam(EObjectTeam OwnerTeam);
	UDataTable* GetImprovementDataByTeam(EObjectTeam OwnerTeam);

private:

	UPROPERTY()
	TMap<EObjectTeam, UDataTable*> AISpawnDataTables;

	UPROPERTY()
	TMap<EObjectTeam, UDataTable*> BuilderDataTables;
	
	UPROPERTY()
	TMap<EObjectTeam, UDataTable*> ImprovementDataTables;
};
