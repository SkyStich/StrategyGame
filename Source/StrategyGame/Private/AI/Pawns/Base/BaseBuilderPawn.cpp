// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Pawns/Base/BaseBuilderPawn.h"


#include "AIController.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Player/UI/ActionSlots/Base/ActionSpawnBuildingSlot.h"
#include "Player/HUD/StrategyGameBaseHUD.h"
#include "GameInstance/Subsystems/GameAIPawnSubsystem.h"
#include "Kismet/GameplayStatics.h"

void ABaseBuilderPawn::HighlightedActor_Implementation(AStrategyGameBaseHUD* PlayerHUD)
{
	auto const DataTable = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>()->GetBuilderDataByTeam(OwnerTeam);
	if(!DataTable) return;

	TArray<UActionBaseSlot*> Slots; 
	for(auto ByArray : DataTable->GetRowNames())
	{
		auto const Data = *DataTable->FindRow<FAllianceBuildersData>(ByArray, *DataTable->GetFullName());
		
		auto const Slot = CreateWidget<UActionSpawnBuildingSlot>(PlayerHUD->GetOwningPlayerController(), PlayerHUD->GetSpawnBuildingSlotClass());
		if(Slot)
		{
			Slot->SetClass(Data.BuildClass);
			Slot->SetIcon(Data.Icon);
			Slot->SetResourcesNeedToBuy(Data.ResourcesNeedToBuy);
			Slot->SetRowName(ByArray);
			Slot->Init();
			Slots.Add(Slot);
		}
	}
	PlayerHUD->CreateActionGrid(Slots);
}

bool ABaseBuilderPawn::GiveOrderToTargetPawn_Implementation(const FVector& LocationToMove, AActor* ActorToMove)
{
	if(!Super::GiveOrderToTargetPawn_Implementation(LocationToMove, ActorToMove)) return false;
	

	if(!ActorToMove)
	{
		if(CurrentRepairActor)
		{
			IRepairInterface::Execute_StopObjectRepair(CurrentRepairActor, 5.f);
			CurrentRepairActor = nullptr;
		}
		return true;
	}
	
	auto const HealthComponent = ActorToMove->FindComponentByClass<UObjectHealthComponent>();
	HealthComponent->OnStopRegeneration.AddDynamic(this, &ABaseBuilderPawn::OnRepairFinish);
	
	if(GetLocalRole() == ROLE_Authority)
	{		
		if(ActorToMove == CurrentRepairActor) return true;
		
		if(!ActorToMove->GetClass()->ImplementsInterface(URepairInterface::StaticClass())) return false;			
		
		CurrentRepairActor = ActorToMove;
		FTimerDelegate TimerDel;
		TimerDel.BindUObject(this, &ABaseBuilderPawn::CheckDistanceToBuilding, ActorToMove);
		GetWorld()->GetTimerManager().SetTimer(CheckDistanceToBuildingHandle, TimerDel, 1.f, true);
		return true;
	}
	return true;
}

void ABaseBuilderPawn::CheckDistanceToBuilding(AActor* ActorToMove)
{
	if(!ActorToMove) return;
	auto const Status = GetController<AAIController>()->GetPathFollowingComponent()->GetStatus();
	if(Status == EPathFollowingStatus::Idle)
	{
		IRepairInterface::Execute_StartObjectRepair(ActorToMove, 5);
		GetWorld()->GetTimerManager().ClearTimer(CheckDistanceToBuildingHandle);
	}
}

void ABaseBuilderPawn::OnRepairFinish()
{
	if(!CurrentRepairActor) return;
	auto const HealthComponent = CurrentRepairActor->FindComponentByClass<UObjectHealthComponent>();
	HealthComponent->OnStopRegeneration.RemoveDynamic(this, &ABaseBuilderPawn::OnRepairFinish);
}

