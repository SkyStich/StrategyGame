// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/SpectatorCameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

USpectatorCameraComponent::USpectatorCameraComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ZoomAlpha = 1.f;
	MinZoomLevel = 0.4f;
	MaxZoomLevel = 1.f;
}

void USpectatorCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView);

	APlayerController* PlayerController = GetPlayerController();
	if(PlayerController)
	{
		DesiredView.DesiredFOV = 30.f;

		/** calculation of camera offset */
		float const CurrentOffset = MinCameraOffset + ZoomAlpha * (MaxCameraOffset - MinCameraOffset);
		
		FVector Pos2 = PlayerController->GetFocalLocation();
		DesiredView.Location = PlayerController->GetFocalLocation() - FixedCameraAngle.Vector() * CurrentOffset;
		DesiredView.Rotation = FixedCameraAngle;
	}
}

void USpectatorCameraComponent::DecreaseZoomLevel()
{
	UpdateZoomAlpha(ZoomAlpha - 0.1f);
}

void USpectatorCameraComponent::IncreaseZoomLevel()
{
	UpdateZoomAlpha(ZoomAlpha + 0.1f);
}

void USpectatorCameraComponent::UpdateZoomAlpha(float const NewAlpha)
{
	ZoomAlpha = FMath::Clamp(NewAlpha, MinZoomLevel, MaxZoomLevel);
}

APlayerController* USpectatorCameraComponent::GetPlayerController()
{
	APawn* OwnerPawn = GetOwnerPawn();
	APlayerController* PlayerController = nullptr;
	if(OwnerPawn)
	{
		PlayerController = Cast<APlayerController>(OwnerPawn->Controller);
	}
	return PlayerController;
}

APawn* USpectatorCameraComponent::GetOwnerPawn()
{
	if(GetOwner())
	{
		return Cast<APawn>(GetOwner());
	}
	UE_LOG(LogActorComponent, Error, TEXT("Owner component is null"), *GetFullName());
	return nullptr;
}
