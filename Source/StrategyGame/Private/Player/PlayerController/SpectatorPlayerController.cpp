// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerController/SpectatorPlayerController.h"

#include "DrawDebugHelpers.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Components/SpectatorCameraComponent.h"
#include "Player/Interfaces/HighlightedInterface.h"
#include "UObject/ConstructorHelpers.h"

ASpectatorPlayerController::ASpectatorPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
	bReplicates = true;
	NetUpdateFrequency = 1.f;
	
	bShowMouseCursor =  true;

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> HighlightedInstanceFinder(TEXT("/Game/Assets/SelectingObject/Materials/PP_Outliner_Inst"));
	if(HighlightedInstanceFinder.Succeeded()) HighlightedMaterialInstance = HighlightedInstanceFinder.Object;
}

void ASpectatorPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() != ROLE_Authority)
	{
		SetInputMode(FInputModeGameOnly());
		SpawnPostProcess();
	}
}

void ASpectatorPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(GetLocalRole() != ROLE_Authority)
	{
		UpdateHighlightedActor();
	}
}

void ASpectatorPlayerController::UpdateRotation(float DeltaTime)
{
	Super::UpdateRotation(DeltaTime);

	FRotator ViewRotation(0.f);
	FRotator DeltaRot(0.f);

	if(PlayerCameraManager)
	{
		/** Write ViewRotation for update control rotation */
		PlayerCameraManager->ProcessViewRotation(DeltaTime, ViewRotation, DeltaRot);
	}
	SetControlRotation(ViewRotation);
}

USpectatorCameraComponent* ASpectatorPlayerController::GetSpectatorCameraComponent()
{
	if(!GetPawnOrSpectator())
	{
		UE_LOG(LogPlayerController, Error, TEXT("--Controlled pawn is null"), *GetFullName());
		return nullptr;
	}
	return GetPawnOrSpectator()->FindComponentByClass<USpectatorCameraComponent>();
}

void ASpectatorPlayerController::ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)
{
	Super::ProcessPlayerInput(DeltaTime, bGamePaused);
	USpectatorCameraComponent* SpectatorCameraComponent = GetSpectatorCameraComponent();
	if(SpectatorCameraComponent)
	{
		SpectatorCameraComponent->UpdateCameraMovement(this);
	}
}

void ASpectatorPlayerController::SpawnPostProcess()
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = GetPawn();
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	APostProcessVolume* PostProcessVolume = GetWorld()->SpawnActor<APostProcessVolume>(FVector(0.f), FRotator(0.f), SpawnParameters);
	if(PostProcessVolume)
	{
		PostProcessVolume->bUnbound = true;		
		PostProcessVolume->Settings.WeightedBlendables.Array.Add(FWeightedBlendable(1.f, HighlightedMaterialInstance));
	}
}

void ASpectatorPlayerController::UpdateHighlightedActor()
{
	static AActor* HighlightedActor;
	FHitResult OutHit;
	
	ETraceTypeQuery const TraceChannel = UCollisionProfile::Get()->ConvertToTraceType(ECC_GameTraceChannel1);
	
	if(GetHitResultUnderCursorByChannel(TraceChannel, true, OutHit) && OutHit.GetActor())
	{
		if(OutHit.GetActor()->GetClass()->ImplementsInterface(UHighlightedInterface::StaticClass()))
		{
			if(HighlightedActor)
			{
				/** return if highlighted actor == current outhit actor */
				if(HighlightedActor == OutHit.GetActor()) return;

				UpdateCustomDepthFromActor(HighlightedActor, false);
			}

			/** Set hilighted actor */
			HighlightedActor = OutHit.GetActor();
			
			UpdateCustomDepthFromActor(HighlightedActor, true);
			DebugTraceFromMousePosition(OutHit);
			return;
		}
	}
	
	DebugTraceFromMousePosition(OutHit);
	
	if(HighlightedActor)
	{
		UpdateCustomDepthFromActor(HighlightedActor, false);
		HighlightedActor = nullptr;	
	}
}

void ASpectatorPlayerController::DebugTraceFromMousePosition(const FHitResult& OutHit)
{
#if UE_EDITOR
	DrawDebugLine(GetWorld(), OutHit.TraceStart, OutHit.TraceEnd, FColor::Blue, false, 0.f);
	DrawDebugSphere(GetWorld(), OutHit.ImpactPoint, 4.f, 6.f, FColor::Yellow, false, 0.f);
#endif
}

void ASpectatorPlayerController::UpdateCustomDepthFromActor(AActor* Actor, bool bState)
{
	/** set custom depth for static mesh */
	TArray<UStaticMeshComponent*> MeshComponents;
	Actor->GetComponents<UStaticMeshComponent>(MeshComponents);
	for(auto const ByArray : MeshComponents)
	{
		ByArray->SetRenderCustomDepth(bState);
	}

	/** set custom depth for skeletal mesh */
	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	Actor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
	for(auto const ByArray : SkeletalMeshComponents)
	{
		ByArray->SetRenderCustomDepth(bState);
	}
}

