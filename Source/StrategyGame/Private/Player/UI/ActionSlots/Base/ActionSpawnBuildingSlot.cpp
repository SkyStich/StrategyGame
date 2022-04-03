// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/ActionSlots/Base/ActionSpawnBuildingSlot.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"
#include "Player/PlayerController/SpectatorPlayerController.h"

void UActionSpawnBuildingSlot::SpawnBuilding()
{
	UResourcesActorComponent* ResourcesActorComponent = GetOwningPlayer()->FindComponentByClass<UResourcesActorComponent>();
	if(!ResourcesActorComponent) return;

	if(!ResourcesActorComponent->EnoughResources(ResourcesNeedToBuy))
	{
		return;
	}

	auto const PC = Cast<ASpectatorPlayerController>(GetOwningPlayer());
	if(PC)
	{
		PC->SpawnPreBuildAction(USyncLoadLibrary::SyncLoadClass<ABaseBuildingActor>(this, SpawnBuildClass), RowName);
	}
}

