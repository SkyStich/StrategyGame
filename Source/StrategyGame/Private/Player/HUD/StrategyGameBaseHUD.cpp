// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/HUD/StrategyGameBaseHUD.h"
#include "Player/UI/MathWidgetBase/BaseMatchWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"

AStrategyGameBaseHUD::AStrategyGameBaseHUD()
{
	bGroupSelectionActive = false;
	
	static ConstructorHelpers::FClassFinder<UBaseMatchWidget>MainWidgetClassFinder(TEXT("/Game/Blueprints/UI/MainWidget/W_MainMatchWidget"));
	if(MainWidgetClassFinder.Succeeded()) MainWidgetClass = MainWidgetClassFinder.Class;	
}

void AStrategyGameBaseHUD::BeginPlay()
{
	Super::BeginPlay();

	CreateMainWidget();
	GetOwningPlayerController()->GetOnNewPawnNotifier().AddUObject(this, &AStrategyGameBaseHUD::OnNewControlledPawn);
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

		TArray<AActor*> OutActors;
		GetActorsInSelectionRectangle(APawn::StaticClass(), StartGroupSelectionPosition, FVector2D(Ox, Oy), OutActors);
		for(auto ByArray : OutActors)
		{
			ASpectatorPlayerController* SpectatorController = GetSpectatorPlayerController();
			if(SpectatorController && ByArray)
			{
				SpectatorController->AddTargetActor(ByArray);
			}
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
	if(!MainWidgetWidget)
	{
		MainWidgetWidget = USyncLoadLibrary::SyncLoadWidget<UBaseMatchWidget>(this, MainWidgetClass, GetOwningPlayerController());
		if(MainWidgetWidget)
		{
			MainWidgetWidget->AddToViewport(1);
		}
	}
}

void AStrategyGameBaseHUD::RemoveMainWidget()
{
	if(MainWidgetWidget)
	{
		MainWidgetWidget->RemoveFromParent();
		MainWidgetWidget = nullptr;
	}
}

void AStrategyGameBaseHUD::CreateActionObjectGrid(TAssetSubclassOf<UUserWidget> Grid)
{
	if(!Grid.IsNull())
	{
		if(ActiveActionGrid)
		{
			RemoveActionObjectGrid();
		}
		ActiveActionGrid = USyncLoadLibrary::SyncLoadWidget<UUserWidget>(this, Grid, GetOwningPlayerController());
		ActiveActionGrid->AddToViewport();
	}
	UE_LOG(LogClass, Error, TEXT("Action object grid is null --%s"), *GetFullName());
}

void AStrategyGameBaseHUD::RemoveActionObjectGrid()
{
	if(ActiveActionGrid)
	{
		ActiveActionGrid->RemoveFromParent();
		ActiveActionGrid = nullptr;
	}
}
