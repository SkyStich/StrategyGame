// Copyright Epic Games, Inc. All Rights Reserved.


#include "StrategyGameGameModeBase.h"

#include "Player/PlayerStates/StrategyMatchPlayerState.h"
#include "Public/Player/Pawn/SpectatorPlayerPawn.h"
#include "Public/Player/PlayerController/SpectatorPlayerController.h"
#include "Public/Player/HUD/StrategyGameBaseHUD.h"

AStrategyGameGameModeBase::AStrategyGameGameModeBase()
{
	DefaultPawnClass = ASpectatorPlayerPawn::StaticClass();
	PlayerControllerClass = ASpectatorPlayerController::StaticClass();
	SpectatorClass = ASpectatorPlayerPawn::StaticClass();
	HUDClass = AStrategyGameBaseHUD::StaticClass();
	PlayerStateClass = AStrategyMatchPlayerState::StaticClass();
	bStartPlayersAsSpectators = true;
}