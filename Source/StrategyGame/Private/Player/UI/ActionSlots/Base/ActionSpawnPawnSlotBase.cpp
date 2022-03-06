// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/ActionSlots/Base/ActionSpawnPawnSlotBase.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"
#include "Player/PlayerController/SpectatorPlayerController.h"
#include "GameInstance/Subsystems/GameAIPawnSubsystem.h"

bool UActionSpawnPawnSlotBase::SpawnPawn()
{
	if(!BuildOwner) return false;
	
	auto const AISubsystem = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>();
	UDataTable* AIDataTable = AISubsystem->GetPawnDataByTeam(BuildOwner->FindObjectTeam_Implementation());
	FAIPawnData* AIData = AIDataTable->FindRow<FAIPawnData>(RowId, TEXT("AIPawnDataTable"));
	if(BuildOwner->GetOwnerController()->GetResourcesActorComponent()->EnoughResources(AIData->ResourcesNeedToBuy))
	{
		BuildOwner->SpawnPawn(RowId);
		return true;
	}
	return false;
}

void UActionSpawnPawnSlotBase::Init_Implementation(ABaseBuildingActor* OwnerBuild, const FName& RowName, const TAssetPtr<UTexture2D>& Icon)
{
	RowId = RowName;
	BuildOwner = OwnerBuild;
}
