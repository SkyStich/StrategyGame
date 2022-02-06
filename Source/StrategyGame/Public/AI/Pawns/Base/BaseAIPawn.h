// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/TeamData.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/FindObjectTeamInterface.h"
#include "Interfaces/GiveOrderToTargetPawns.h"
#include "Player/Interfaces/HighlightedInterface.h"
#include "BaseAIPawn.generated.h"

UCLASS()
class STRATEGYGAME_API ABaseAIPawn : public APawn, public IHighlightedInterface, public IGiveOrderToTargetPawns, public IFindObjectTeamInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseAIPawn();

	virtual void GiveOrderToTargetPawn_Implementation(const FVector& LocationToMove, AActor* ActorToMove) override;
	void SetTeam(EObjectTeam Team)  { OwnerTeam = Team; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual EObjectTeam FindObjectTeam_Implementation() override { return OwnerTeam; }

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCapsuleComponent* PawnCapsuleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMeshComponent* SkeletalMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UFloatingPawnMovement* PawnMovementComponent;

	UPROPERTY(Replicated, EditAnyWhere)
	EObjectTeam OwnerTeam;
};
