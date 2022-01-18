// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/Building/BuildingGridBase.h"
#include "BlueprintFunctionLibraries/AsyncLoadLibrary.h"
#include "Player/UI/Building/BuildingSlotBase.h"
#include "Actors/StaticMeshActors/PreBuildingActor.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerController/SpectatorPlayerController.h"

void UBuildingGridBase::CreateBuildingSlots(UUniformGridPanel* UniformGridPanel)
{
	static const FString ContextString("DT_BuildingObjectInfo");
	TArray<FBuildingObjectInfo*> DataStructs;
	BuildingDataTable->GetAllRows<FBuildingObjectInfo>(ContextString, DataStructs);
	
	for(int i = 0; i < DataStructs.Num(); i++)
	{
		if(DataStructs[i])
		{
			UBuildingSlotBase* BuildingSlot = USyncLoadLibrary::SyncLoadWidget<UBuildingSlotBase>(this, BuildingSlotBase, GetOwningPlayer());
			if(BuildingSlot)
			{
				BuildingSlot->SetSlotInfo(DataStructs[i]);
				BuildingSlot->OnBuildingSlotClicked.BindUFunction(this, "OnBuildingSlotClickedEvent");
				BuildingSlot->AddToViewport();
				UniformGridPanel->AddChildToUniformGrid(BuildingSlot, i / 4, i % 4);
			}
		}
	} 
}

void UBuildingGridBase::OnBuildingSlotClickedEvent(const FBuildingObjectInfo& SlotClicked)
{
	TSubclassOf<ABaseBuildingActor> SpawnClass = USyncLoadLibrary::SyncLoadClass<ABaseBuildingActor>(this, SlotClicked.BuildingClass);
	if(SpawnClass)
	{
		auto const SubobjectMesh = Cast<UStaticMeshComponent>(SpawnClass->GetDefaultSubobjectByName("StaticMesh"));
		auto const SubobjectBox = Cast<UBoxComponent>(SpawnClass->GetDefaultSubobjectByName("BoxComponent"));
		if(SubobjectMesh && SubobjectBox)
		{
			APreBuildingActor* PreBuildingActor = nullptr;
			FActorSpawnParameters Param;
			Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			PreBuildingActor = GetWorld()->SpawnActor<APreBuildingActor>(APreBuildingActor::StaticClass(), Param);
			if(PreBuildingActor)
			{
				auto const PC = Cast<ASpectatorPlayerController>(GetOwningPlayer());
				PC->bIgnoreHighlighted = true;
				
				PreBuildingActor->GetStaticMeshComponent()->SetStaticMesh(SubobjectMesh->GetStaticMesh());
				PreBuildingActor->GetStaticMeshComponent()->SetRelativeScale3D(SubobjectMesh->GetRelativeScale3D());
				PreBuildingActor->SetBoxExtent(SubobjectBox->GetScaledBoxExtent());
				PreBuildingActor->SetOwnerController(PC);
				PreBuildingActor->SetBuildingActor(SpawnClass);
				
				UGameplayStatics::FinishSpawningActor(PreBuildingActor, FTransform(FVector(0.f)));
			}
		}
	}
}

void UBuildingGridBase::OnLoadBuildingClassInMemoryComplete(bool bResult, const FString& Reference, TSubclassOf<AActor> Building)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Loaded"));	
}

