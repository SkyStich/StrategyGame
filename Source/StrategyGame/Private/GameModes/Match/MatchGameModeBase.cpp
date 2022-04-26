// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/Match/MatchGameModeBase.h"
#include "EngineUtils.h"
#include "GameFramework/GameState.h"
#include "Actors/SpawnLocation/BeginSpawnLocation.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"
#include "GameInstance/Subsystems/GameAIPawnSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PlayerStates/StrategyMatchPlayerState.h"
#include "Player/Pawn/SpectatorPlayerPawn.h"
#include "Player/PlayerController/SpectatorPlayerController.h"
#include "Player/HUD/StrategyGameBaseHUD.h"

AMatchGameModeBase::AMatchGameModeBase()
{
	DefaultPawnClass = ASpectatorPlayerPawn::StaticClass();
	PlayerControllerClass = ASpectatorPlayerController::StaticClass();
	SpectatorClass = ASpectatorPlayerPawn::StaticClass();
	HUDClass = AStrategyGameBaseHUD::StaticClass();
	PlayerStateClass = AStrategyMatchPlayerState::StaticClass();
	bStartPlayersAsSpectators = true;
}

void AMatchGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	///Test logic!
	auto const DataTable = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>()->GetBuilderDataByTeam(EObjectTeam::TeamA);
	auto const Row = DataTable->FindRow<FAllianceBuildersData>("MainStation", *DataTable->GetFullName());
	USyncLoadLibrary::SyncLoadClass(this, Row->BuildClass);

	FTimerDelegate TimerDel;
	TimerDel.BindLambda([&]() -> void
	{
		InitPlayerBuilding();
	});
	FTimerHandle Timer;
	GetWorld()->GetTimerManager().SetTimer(Timer, TimerDel, 5.f, false);
}

/** Only debug logic */
void AMatchGameModeBase::InitPlayerBuilding()
{
	for(TActorIterator<ABeginSpawnLocation> It(GetWorld(), ABeginSpawnLocation::StaticClass()); It; ++It)
	{
		ABeginSpawnLocation* Temp = *It;

		ABaseBuildingActor* MainStation = nullptr;
		if(Temp->GetSpawnIndex() == 0)
		{
			APlayerController* Controller = nullptr;
			for(auto& ByArray : GameState->PlayerArray)
			{
				Controller = Cast<APlayerController>(ByArray->GetOwner());
				break;
			}
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParameters.Owner = Controller;
			SpawnParameters.Instigator = Controller->GetPawnOrSpectator();
			
			auto const DataTable = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>()->GetBuilderDataByTeam(EObjectTeam::TeamA);
			auto const Row = DataTable->FindRow<FAllianceBuildersData>("MainStation", *DataTable->GetFullName());
			auto const Class = USyncLoadLibrary::SyncLoadClass(Controller, Row->BuildClass);
			MainStation = GetWorld()->SpawnActor<ABaseBuildingActor>(Class, Temp->GetActorLocation(), FRotator::ZeroRotator, SpawnParameters);
			MainStation->SetOwnerController(Cast<ASpectatorPlayerController>(Controller));
			MainStation->SetTeamOwner(EObjectTeam::TeamA);
			MainStation->SetBuildName(Row->DisplayName);
			MainStation->InstantObjectInstallation(Row->MaxHealth);
			Temp->Destroy(true);
			return;
		}
	}
}
