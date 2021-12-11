// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HUD/StrategyGameBaseHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"

AStrategyGameBaseHUD::AStrategyGameBaseHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget>BuildingGridFinder(TEXT("/Game/Blueprints/UI/Building/W_BuildingGrid"));
	if(BuildingGridFinder.Succeeded()) BuildingGridClassPtr = BuildingGridFinder.Class;	
}

void AStrategyGameBaseHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateBuildingGrid();
	GetOwningPlayerController()->GetOnNewPawnNotifier().AddUObject(this, &AStrategyGameBaseHUD::OnNewControlledPawn);
}

void AStrategyGameBaseHUD::OnNewControlledPawn(APawn* NewPawn)
{
	if(GetOwningPlayerController()->GetSpectatorPawn())
	{
		/** if controllered spectator */
		CreateBuildingGrid();
	}
	else if(NewPawn)
	{
		/** if player controlled AI Pawn */
	}
}

void AStrategyGameBaseHUD::CreateBuildingGrid()
{
	if(!BuildingGridWidget)
	{
		BuildingGridWidget = USyncLoadLibrary::SyncLoadWidget<UUserWidget>(this, BuildingGridClassPtr, GetOwningPlayerController());
		if(BuildingGridWidget) BuildingGridWidget->AddToViewport();
	}
}

void AStrategyGameBaseHUD::RemoveBuildingGrid()
{
	if(BuildingGridWidget)
	{
		BuildingGridWidget->RemoveFromParent();
		BuildingGridWidget = nullptr;
	}
}