// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/Subsystems/GameAIPawnSubsystem.h"

UGameAIPawnSubsystem::UGameAIPawnSubsystem()
{
	/** finder pawn data table */
	static ConstructorHelpers::FObjectFinder<UDataTable> AISpawnDataTableFinder(TEXT("/Game/Blueprints/DataTables/DT_AlliancePawnData"));
    if(AISpawnDataTableFinder.Succeeded()) AISpawnDataTables.Add(EObjectTeam::TeamA, AISpawnDataTableFinder.Object);

	/** finder builder data table */
	static ConstructorHelpers::FObjectFinder<UDataTable> BuilderDataFinder(TEXT("/Game/Blueprints/DataTables/DT_AllianceBuilderData"));
	if(BuilderDataFinder.Succeeded()) BuilderDataTables.Add(EObjectTeam::TeamA, BuilderDataFinder.Object);

	/** finder improvement data table */
	static ConstructorHelpers::FObjectFinder<UDataTable> ImprovementDataFinder(TEXT("/Game/Blueprints/DataTables/DT_AllianceImprovementData"));
	if(ImprovementDataFinder.Succeeded()) ImprovementDataTables.Add(EObjectTeam::TeamA, ImprovementDataFinder.Object);
}

UDataTable* UGameAIPawnSubsystem::GetBuilderDataByTeam(EObjectTeam OwnerTeam)
{
	return BuilderDataTables.FindRef(OwnerTeam);
}

UDataTable* UGameAIPawnSubsystem::GetPawnDataByTeam(EObjectTeam OwnerTeam)
{
	return AISpawnDataTables.FindRef(OwnerTeam);
}

UDataTable* UGameAIPawnSubsystem::GetImprovementDataByTeam(EObjectTeam OwnerTeam)
{
	return ImprovementDataTables.FindRef(OwnerTeam);
}

