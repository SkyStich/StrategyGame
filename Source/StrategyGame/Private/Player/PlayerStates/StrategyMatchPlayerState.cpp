// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerStates/StrategyMatchPlayerState.h"
#include "Net/UnrealNetwork.h"

AStrategyMatchPlayerState::AStrategyMatchPlayerState()
{
	PlayerTeam = EObjectTeam::TeamA;
}

void AStrategyMatchPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AStrategyMatchPlayerState, PlayerTeam);
}
