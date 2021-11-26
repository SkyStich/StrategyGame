// Copyright Epic Games, Inc. All Rights Reserved.


#include "StrategyGameGameModeBase.h"
#include "Public/Player/Pawn/SpectatorPlayerPawn.h"
#include "Public/Player/PlayerController/SpectatorPlayerController.h"

AStrategyGameGameModeBase::AStrategyGameGameModeBase()
{
	DefaultPawnClass = ASpectatorPlayerPawn::StaticClass();
	PlayerControllerClass = ASpectatorPlayerController::StaticClass();
}