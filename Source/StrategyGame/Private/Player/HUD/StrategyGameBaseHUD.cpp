// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HUD/StrategyGameBaseHUD.h"
#include "Player/UI/MathWidgetBase/BaseMatchWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"
#include "Player/PlayerStates/StrategyMatchPlayerState.h"
#include "Player/UI/Building/BuildingGridBase.h"
#include "Player/UI/Building/BuildingSlotBase.h"

AStrategyGameBaseHUD::AStrategyGameBaseHUD()
{
	bGroupSelectionActive = false;
	
	static ConstructorHelpers::FClassFinder<UBaseMatchWidget>MainWidgetClassFinder(TEXT("/Game/Blueprints/UI/MainWidget/W_MainMatchWidget"));
	if(MainWidgetClassFinder.Succeeded()) MainWidgetClass = MainWidgetClassFinder.Class;

	static ConstructorHelpers::FClassFinder<UBuildingGridBase>BuildingGridFinder(TEXT("/Game/Blueprints/UI/Actions/Grid/BP_ActionGrid"));
	if(BuildingGridFinder.Succeeded()) ActionGridClass = BuildingGridFinder.Class;
}

void AStrategyGameBaseHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateMainWidget();
	GetOwningPlayerController()->GetOnNewPawnNotifier().AddUObject(this, &AStrategyGameBaseHUD::OnNewControlledPawn);
}

void AStrategyGameBaseHUD::GroupSelectingReleased()
{
	float Ox , Oy;	
	GetOwningPlayerController()->GetMousePosition(Ox, Oy);
	
	TArray<AActor*> OutActors;
	GetActorsInSelectionRectangle(APawn::StaticClass(), StartGroupSelectionPosition, FVector2D(Ox, Oy), OutActors);

	if(OutActors.Num() <= 0) return;
	
	ASpectatorPlayerController* SpectatorController = GetSpectatorPlayerController();
	if(SpectatorController)
	{
		/** find command first item who have team and remove items who have not team */
		EObjectTeam FirstElementTeam = EObjectTeam::None;
		for(const auto& ByArray : OutActors)
		{
			/** remove item if he not have team */
			if(!ByArray->GetClass()->ImplementsInterface(UFindObjectTeamInterface::StaticClass()))
			{
				OutActors.Remove(ByArray);
				continue;
			}

			/** write team first item who have team */
			if(FirstElementTeam != EObjectTeam::None)
			{
				FirstElementTeam = IFindObjectTeamInterface::Execute_FindObjectTeam(ByArray);
			}
		}
		SpectatorController->Server_AddTargetActors(OutActors);
	}
}

void AStrategyGameBaseHUD::DrawHUD()
{
	Super::DrawHUD();

	if(bGroupSelectionActive)
	{
		float Ox , Oy;
		float const StartX = StartGroupSelectionPosition.X;
		float const StartY = StartGroupSelectionPosition.Y;		
		GetOwningPlayerController()->GetMousePosition(Ox, Oy);

		if(FVector2D(Ox, Oy) == StartGroupSelectionPosition) return;
		
		/** draw left line */
		DrawLine(StartX, StartY, StartX, Oy, FColor::Black, 0.f);

		/** Draw right line */
		DrawLine(StartX, StartY, Ox, StartY, FColor::Black, 0.f);

		/** Draw top line */
		DrawLine(Ox, StartY, Ox, Oy, FColor::Black, 0.f);

		/** Draw bot line */
		DrawLine(StartX, Oy, Ox, Oy, FColor::Black, 0.f);
	}
}

ASpectatorPlayerController* AStrategyGameBaseHUD::GetSpectatorPlayerController()
{
	ASpectatorPlayerController* Controller = nullptr;
	if(GetOwningPlayerController())
	{
		Controller = Cast<ASpectatorPlayerController>(GetOwningPlayerController());
	}
	return Controller;
}

void AStrategyGameBaseHUD::OnNewControlledPawn(APawn* NewPawn)
{
	if(GetOwningPlayerController()->GetSpectatorPawn())
	{
		/** if controllered spectator */
		CreateMainWidget();
	}
	else if(NewPawn)
	{
		/** if player controlled AI Pawn */
	}
}

void AStrategyGameBaseHUD::CreateMainWidget()
{
	if(!MainWidget)
	{
		MainWidget = USyncLoadLibrary::SyncLoadWidget<UBaseMatchWidget>(this, MainWidgetClass, GetOwningPlayerController());
		if(MainWidget)
		{
			MainWidget->AddToViewport(0);
		}
	}
}

void AStrategyGameBaseHUD::RemoveMainWidget()
{
	if(MainWidget)
	{
		MainWidget->RemoveFromParent();
		MainWidget = nullptr;
	}
}

void AStrategyGameBaseHUD::CreateActionGrid(const TArray<UActionBaseSlot*>& Slots)
{
	if(Slots.Num() <= 0 || ActionGridClass.IsNull()) return;
	
	if(ActionGrid)
	{
		ClearActionGrid();
	}
	else
	{
		ActionGrid = USyncLoadLibrary::SyncLoadWidget<UBuildingGridBase>(this, ActionGridClass, GetOwningPlayerController());
		ActionGrid->AddToViewport();
		if(!MainWidget)
		{
			CreateMainWidget();
		}
		MainWidget->AttachWidgetToLeftBorder(ActionGrid);
	}
	ActionGrid->Init(Slots);
	
}

void AStrategyGameBaseHUD::ClearActionGrid()
{
	if(ActionGrid)
	{
		ActionGrid->ClearGrid();
	}
}

void AStrategyGameBaseHUD::RemoveActionGrid()
{
	if(ActionGrid)
	{
		ActionGrid->RemoveFromParent();
		ActionGrid = nullptr;
	}
}
