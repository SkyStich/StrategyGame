// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/Subsystems/GameAIPawnSubsystem.h"

UGameAIPawnSubsystem::UGameAIPawnSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> AISpawnDataTableFinder(TEXT("/Game/Blueprints/DataTables/DT_AlliancePawnData"));
    if(AISpawnDataTableFinder.Succeeded()) AISpawnDataTables.Add(EObjectTeam::TeamA, AISpawnDataTableFinder.Object);
}

UDataTable* UGameAIPawnSubsystem::GetPawnDataByTeam(EObjectTeam OwnerTeam)
{
	return AISpawnDataTables.FindRef(OwnerTeam);
}
