// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Objects/PlayerImprovementCharacteristics.h"
#include "Player/PlayerController/SpectatorPlayerController.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"

UPlayerImprovementCharacteristics::UPlayerImprovementCharacteristics(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

bool UPlayerImprovementCharacteristics::IsAuthority() const
{
	return GetWorld() == nullptr || Owner->GetNetMode() != NM_Client || GetWorld()->IsPlayingReplay();
}

void UPlayerImprovementCharacteristics::PostInitProperties()
{
	Super::PostInitProperties();

	if(GetOuter() && GetOuter()->GetWorld()) BeginPlay();
}

void UPlayerImprovementCharacteristics::BeginPlay()
{
	
}

int32 UPlayerImprovementCharacteristics::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	return Owner ? Owner->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local;
}

bool UPlayerImprovementCharacteristics::CallRemoteFunction(UFunction* Function, void* Parms, FOutParmRec* OutParms, FFrame* Stack)
{
	if(!Owner) return false;

	UNetDriver* NetDriver = Owner->GetNetDriver();
	if(!NetDriver) return false;

	NetDriver->ProcessRemoteFunction(Owner, Function, Parms, OutParms, Stack, this);
	return true;
}
