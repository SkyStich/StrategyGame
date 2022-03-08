// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HUD/StrategyGameBaseHUD.h"

#include "AI/Pawns/Base/BaseAggressivePawn.h"
#include "Player/UI/MathWidgetBase/BaseMatchWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"
#include "AI/Pawns/Base/BaseAIPawn.h"
#include "Player/PlayerStates/StrategyMatchPlayerState.h"
#include "Player/UI/ActionGrid/ActionGridBase.h"

AStrategyGameBaseHUD::AStrategyGameBaseHUD()
{
	bGroupSelectionActive = false;
	bGroupSelection = false;
	
	static ConstructorHelpers::FClassFinder<UBaseMatchWidget>MainWidgetClassFinder(TEXT("/Game/Blueprints/UI/MainWidget/W_MainMatchWidget"));
	if(MainWidgetClassFinder.Succeeded()) MainWidgetClass = MainWidgetClassFinder.Class;

	static ConstructorHelpers::FClassFinder<UActionGridBase>ActionGridFinder(TEXT("/Game/Blueprints/UI/Actions/Grid/W_ActionGrud"));
	if(ActionGridFinder.Succeeded()) ActionGridClass = ActionGridFinder.Class;

	static ConstructorHelpers::FClassFinder<UActionSpawnPawnSlotBase>SpawnPawnSlotFinder(TEXT("/Game/Blueprints/UI/Actions/W_SpawnPawnSlot"));
	if(SpawnPawnSlotFinder.Succeeded()) ActionSpawnPawnSlotClass = SpawnPawnSlotFinder.Class;

	static ConstructorHelpers::FClassFinder<UHealthStatisticsBase> HealthStatisticsFinder(TEXT("/Game/Blueprints/UI/HealthState/W_HealthStateWidget"));
	if(HealthStatisticsFinder.Succeeded()) HealthStatisticsWidgetClass = HealthStatisticsFinder.Class;
}

void AStrategyGameBaseHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateMainWidget();
	CreateHealthStatisticsWidget();
	GetOwningPlayerController()->GetOnNewPawnNotifier().AddUObject(this, &AStrategyGameBaseHUD::OnNewControlledPawn);

	auto const Controller = Cast<ASpectatorPlayerController>(GetOwningPlayerController());
	if(Controller)
	{
		Controller->OnActionWithObjectReleasedEvent.AddDynamic(this, &AStrategyGameBaseHUD::OnActionWithObjectReleased);
	}
}

void AStrategyGameBaseHUD::OnActionWithObjectReleased()
{
	float Ox, Oy;
	GetOwningPlayerController()->GetMousePosition(Ox, Oy);
	
	if(!FVector2D(Ox, Oy).Equals(StartGroupSelectionPosition, 25.f)) bGroupSelection = true;
	else SetGroupSelectionActive(false);
}

void AStrategyGameBaseHUD::GroupSelectingReleased()
{
	float Ox , Oy;	
	GetOwningPlayerController()->GetMousePosition(Ox, Oy);
	
	TArray<ABaseAIPawn*> OutActors;
	if(!GetActorsInSelectionRectangle(StartGroupSelectionPosition, FVector2D(Ox, Oy), OutActors))
	{
		return;
	}
	
	ASpectatorPlayerController* SpectatorController = GetSpectatorPlayerController();
	EObjectTeam const OwnerTeam = IFindObjectTeamInterface::Execute_FindObjectTeam(GetOwningPlayerController());

	/** array for filter aggressive pawn */
	TArray<ABaseAIPawn*> AggressivePawnArray;
	if(SpectatorController)
	{
		for(auto ByArray : OutActors)
		{		
			if(OwnerTeam != IFindObjectTeamInterface::Execute_FindObjectTeam(ByArray))
			{
				OutActors.Remove(ByArray);
				continue;
			}
			if(Cast<ABaseAggressivePawn>(ByArray)) AggressivePawnArray.Add(ByArray);
		}
		SpectatorController->AddTargetPawns(AggressivePawnArray.Num() > 0 ? AggressivePawnArray : OutActors);
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

		if(FVector2D(Ox, Oy).Equals(StartGroupSelectionPosition, 25.f)) return;
		
		/** draw left line */
		DrawLine(StartX, StartY, StartX, Oy, FColor::Black, 0.f);

		/** Draw right line */
		DrawLine(StartX, StartY, Ox, StartY, FColor::Black, 0.f);

		/** Draw top line */
		DrawLine(Ox, StartY, Ox, Oy, FColor::Black, 0.f);

		/** Draw bot line */
		DrawLine(StartX, Oy, Ox, Oy, FColor::Black, 0.f);

		if(bGroupSelection)
		{
			bGroupSelection = false;
			GroupSelectingReleased();
			SetGroupSelectionActive(false);
			StartGroupSelectionPosition = FVector2D::ZeroVector;
		}
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
		ActionGrid = USyncLoadLibrary::SyncLoadWidget<UActionGridBase>(this, ActionGridClass, GetOwningPlayerController());
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

void AStrategyGameBaseHUD::CreateHealthStatisticsWidget()
{
	if(!HealthStatisticsWidget)
	{
		HealthStatisticsWidget = USyncLoadLibrary::SyncLoadWidget<UHealthStatisticsBase>(this, HealthStatisticsWidgetClass, GetOwningPlayerController());
		HealthStatisticsWidget->AddToViewport();
		HealthStatisticsWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AStrategyGameBaseHUD::RemoveHealthStatisticsWidget()
{
	if(HealthStatisticsWidget) HealthStatisticsWidget->RemoveFromParent();
}

void AStrategyGameBaseHUD::ShowHealthStatistics(AActor* Target)
{
	if(!Target) return;
	
	if(!HealthStatisticsWidget)
	{
		CreateHealthStatisticsWidget();
	}
	HealthStatisticsWidget->SetTarget(Target);
	HealthStatisticsWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void AStrategyGameBaseHUD::HiddenHealthStatistics()
{
	if(HealthStatisticsWidget)
	{
		HealthStatisticsWidget->ClearTarget();
		HealthStatisticsWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

