// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enums/TeamData.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/GiveOrderToTargetPawns.h"
#include "Player/Interfaces/HighlightedInterface.h"
#include "BaseAIPawn.generated.h"

UCLASS()
class STRATEGYGAME_API ABaseAIPawn : public APawn, public IHighlightedInterface, public IGiveOrderToTargetPawns
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

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(Replicated, EditAnyWhere)
	EObjectTeam OwnerTeam;
};
