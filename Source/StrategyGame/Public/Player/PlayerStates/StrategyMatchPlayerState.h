// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Enums/TeamData.h"
#include "StrategyMatchPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API AStrategyMatchPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	AStrategyMatchPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

	/** get team owner's helper */
	EObjectTeam GetPlayerTeam() const { return PlayerTeam; }

	/**
	 * Write player team 
	 * @param	Team	New player team
	 */
	void SetPlayerTeam(const EObjectTeam Team) { if(GetLocalRole() == ROLE_Authority) PlayerTeam = Team; } 
	
private:

	UPROPERTY(Replicated)
	EObjectTeam PlayerTeam;
};
