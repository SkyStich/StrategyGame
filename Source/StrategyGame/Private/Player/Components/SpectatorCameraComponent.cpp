// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/SpectatorCameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpectatorPawn.h"
#include "Player/Components/PlayerSpectatorPawnMovement.h"

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

		/** calculate camera location and rotation */
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
	UE_LOG(LogActorComponent, Error, TEXT("--Owner component is null"), *GetFullName());
	return nullptr;
}

void USpectatorCameraComponent::MoveForward(const float Value)
{
	if(Value == 0) return;
	
	APawn* OwnerPawn = GetOwnerPawn();
	if(OwnerPawn)
	{
		APlayerController* PlayerController = GetPlayerController();
		if(PlayerController)
		{
			FRotator const CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();

			/** Install by pitch on Zero because camera is turned by 45 units */
			FRotationMatrix const RotateMatrix(FRotator(0.f, CameraRotation.Yaw, CameraRotation.Roll));
			FVector const WorldSpaceAccess = RotateMatrix.GetScaledAxis(EAxis::X) * 1000.f;

			/** transform to world space and add it */
			OwnerPawn->AddMovementInput(WorldSpaceAccess, Value);
		}
	}
}

void USpectatorCameraComponent::MoveRight(const float Value)
{
	if(Value == 0) return;

	APawn* OwnerPawn = GetOwnerPawn();
	if(OwnerPawn)
	{
		APlayerController* PlayerController = GetPlayerController();
		if(PlayerController)
		{
			FRotationMatrix const RotateMatrix(PlayerController->PlayerCameraManager->GetCameraRotation());
			FVector const WorldSpaceAccess = RotateMatrix.GetScaledAxis(EAxis::Y) * 1000.f;

			/** Transform to world space and add it */
			OwnerPawn->AddMovementInput(WorldSpaceAccess, Value);
		}
	}
}

void USpectatorCameraComponent::UpdateCameraMovement(APlayerController* InPlayerController)
{
	/** on mouse supported */
#if PLATFORM_DESKTOP

	ULocalPlayer* const LocalPlayer = Cast<ULocalPlayer>(InPlayerController->Player);
	if(LocalPlayer && LocalPlayer->ViewportClient && LocalPlayer->ViewportClient->Viewport)
	{
		FVector2D MousePosition(0.f);
		if(!LocalPlayer->ViewportClient->GetMousePosition(MousePosition)) return;

		FViewport* Viewport = LocalPlayer->ViewportClient->Viewport;
		float const ScrollSpeed = 60.f;
		const FIntPoint ViewportSize = Viewport->GetSizeXY();

		const uint32 ViewLeft = FMath::TruncToInt(LocalPlayer->Origin.X * ViewportSize.X);
		const uint32 ViewRight = ViewLeft + FMath::TruncToInt(LocalPlayer->Size.X * ViewportSize.X);
		const uint32 ViewTop = FMath::TruncToInt(LocalPlayer->Origin.Y * ViewportSize.Y);
		const uint32 ViewBottom = ViewTop + FMath::TruncToInt(LocalPlayer->Size.Y * ViewportSize.Y);

		float const MaxCameraSpeed = CameraScrollSpeed * ZoomAlpha;
		FVector MouseCoords(MousePosition, 0.f);
		const uint32 MouseX = MousePosition.X;
		const uint32 MouseY = MousePosition.Y;
		float SpectatorCameraSpeed = MaxCameraSpeed;
		ASpectatorPawn* SpectatorPawn = nullptr;
		APlayerController* PlayerController = GetPlayerController();
		if(PlayerController)
		{
			SpectatorPawn = PlayerController->GetSpectatorPawn();
			if(SpectatorPawn->GetMovementComponent())
			{
				SpectatorCameraSpeed = GetDefault<USpectatorPawnMovement>(SpectatorPawn->GetMovementComponent()->GetClass())->MaxSpeed;
			}
		}

		/** Move right */
		if(MouseX >= ViewLeft && MouseX <= (ViewLeft + CameraActiveBorder))
		{
			const float Delta = 1.f - float(MouseX - ViewLeft) / CameraActiveBorder;
			MoveRight(-ScrollSpeed * Delta);
		}
		else if(MouseX <= ViewRight && MouseX >= (ViewRight - CameraActiveBorder))
		{
			const float Delta = 1.f - float(MouseX - ViewRight + CameraActiveBorder) / CameraActiveBorder;
			MoveRight(ScrollSpeed * Delta);
		}

		/** Move forward */
		if(MouseY >= ViewTop && MouseY <= (ViewTop + CameraActiveBorder))
		{
			const float Delta = 1.f - float(MouseY - ViewTop) / CameraActiveBorder;
			MoveForward(ScrollSpeed * Delta);
		}
		else if(MouseY <= ViewBottom && MouseY >= (ViewBottom - CameraActiveBorder))
		{
			const float Delta = 1.f - float(MouseY - ViewBottom + CameraActiveBorder) / CameraActiveBorder;
			MoveForward(-ScrollSpeed * Delta);
		}

		if(SpectatorPawn)
		{
			UFloatingPawnMovement* PawnMovement = Cast<UFloatingPawnMovement>(SpectatorPawn->GetMovementComponent());
			if(PawnMovement)
			{
				PawnMovement->MaxSpeed = SpectatorCameraSpeed;
			}
		}
	}

#endif
}
