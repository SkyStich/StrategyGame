// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/TeamData.h"
#include "GameFramework/Character.h"
#include "Interfaces/FindObjectTeamInterface.h"
#include "Interfaces/GiveOrderToTargetPawns.h"
#include "Player/Interfaces/HighlightedInterface.h"
#include "BaseAIPawn.generated.h"

UCLASS(Abstract)
class STRATEGYGAME_API ABaseAIPawn : public ACharacter, public IHighlightedInterface, public IGiveOrderToTargetPawns, public IFindObjectTeamInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseAIPawn();

	virtual void GiveOrderToTargetPawn_Implementation(const FVector& LocationToMove, AActor* ActorToMove) override;
	void SetTeam(EObjectTeam Team)  { OwnerTeam = Team; }
	EObjectTeam GetTeam() const { return OwnerTeam; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual EObjectTeam FindObjectTeam_Implementation() override { return OwnerTeam; }

protected:

	UPROPERTY(Replicated, EditAnyWhere)
	EObjectTeam OwnerTeam;
};
