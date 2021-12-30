// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerController/SpectatorPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Components/SpectatorCameraComponent.h"
#include "Player/Interfaces/HighlightedInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Player/HUD/StrategyGameBaseHUD.h"

ASpectatorPlayerController::ASpectatorPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bShowMouseCursor =  true;
	bIgnoreHighlighted = false;
	bReplicates = true;
	NetUpdateFrequency = 1.f;

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

void ASpectatorPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASpectatorPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAction("ActionWithObject", IE_Released, this, &ASpectatorPlayerController::OnSelectActorReleased);
	InputComponent->BindAction("ActionWithObject", IE_Pressed, this, &ASpectatorPlayerController::OnActionWithObjectPressed);
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

void ASpectatorPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if(GetLocalRole() != ROLE_Authority) OnNewPawn.Broadcast(GetPawn());
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

AStrategyGameBaseHUD* ASpectatorPlayerController::GetStrategyGameBaseHUD() const
{
	return GetHUD<AStrategyGameBaseHUD>();
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
	
	ETraceTypeQuery const TraceChannel = UCollisionProfile::Get()->ConvertToTraceType(bIgnoreHighlighted ? ECC_Camera : ECC_GameTraceChannel1);
	
	if(GetHitResultUnderCursorByChannel(TraceChannel, true, MousePositionResult) && MousePositionResult.GetActor())
	{
		if(MousePositionResult.GetActor()->GetClass()->ImplementsInterface(UHighlightedInterface::StaticClass()))
		{
			if(HighlightedActor)
			{
				/** return if highlighted actor == current outhit actor */
				if(HighlightedActor == MousePositionResult.GetActor()) return;

				UpdateCustomDepthFromActor(HighlightedActor, false);
			}

			/** Set hilighted actor */
			HighlightedActor = MousePositionResult.GetActor();
			
			UpdateCustomDepthFromActor(HighlightedActor, true);
			DebugTraceFromMousePosition(MousePositionResult);
			return;
		}
	}
	
	DebugTraceFromMousePosition(MousePositionResult);
	
	/** if the object is no longer selected, clear the static variable and turn it off custom depth  */
	if(HighlightedActor)
	{
		UpdateCustomDepthFromActor(HighlightedActor, false);
		HighlightedActor = nullptr;	
	}
}

#if UE_EDITOR
void ASpectatorPlayerController::DebugTraceFromMousePosition(const FHitResult& OutHit)
{
	DrawDebugLine(GetWorld(), OutHit.TraceStart, OutHit.TraceEnd, FColor::Blue, false, 0.f);
	DrawDebugSphere(GetWorld(), OutHit.ImpactPoint, 72.f, 8.f, FColor::Yellow, false, 0.f);
}
#endif

void ASpectatorPlayerController::UpdateCustomDepthFromActor(AActor* Actor, bool bState)
{
	if(TargetActors.Contains(Actor)) return;
	
	/** set custom depth for static mesh */
	TArray<UStaticMeshComponent*> MeshComponents;
	Actor->GetComponents<UStaticMeshComponent>(MeshComponents);
		
	for(auto const TempStaticMesh : MeshComponents)
	{
		TempStaticMesh->SetRenderCustomDepth(bState);
	}

	/** set custom depth for skeletal mesh */
	TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
	Actor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
	for(auto const TempSkeletalMesh : SkeletalMeshComponents)
	{
		TempSkeletalMesh->SetRenderCustomDepth(bState);
	}
}

void ASpectatorPlayerController::AddTargetActor(AActor* NewTarget)
{
	if(TargetActors.Contains(NewTarget)) return;
	
	TargetActors.Add(NewTarget);
	if(NewTarget)
	{
		IHighlightedInterface::Execute_HighlightedActor(NewTarget, this);
	}
	
	TargetActorUpdated.Broadcast(NewTarget);
}

void ASpectatorPlayerController::ClearTargetActors()
{
	for(auto ByArray : TargetActors)
	{
		TargetActors.Remove(ByArray);
		UpdateCustomDepthFromActor(ByArray, false);
	}
}

void ASpectatorPlayerController::OnSelectActorReleased()
{
	FHitResult OutHit;
	
	ETraceTypeQuery const TraceChannel = UCollisionProfile::Get()->ConvertToTraceType(bIgnoreHighlighted ? ECC_Camera : ECC_GameTraceChannel1);
	
	if(GetHitResultUnderCursorByChannel(TraceChannel, true, OutHit) && OutHit.GetActor())
	{
		if(OutHit.GetActor()->GetClass()->ImplementsInterface(UHighlightedInterface::StaticClass()))
		{
			if(!TargetActors.Contains(OutHit.GetActor()))
			{
				if(TargetActors.Num() > 0) ClearTargetActors();
				
				AddTargetActor(OutHit.GetActor());
			}
		}
	}	
}

void ASpectatorPlayerController::OnActionWithObjectPressed()
{
	if(bIgnoreHighlighted) OnActionWithObjectPressedEvent.Broadcast();
}

void ASpectatorPlayerController::SpawnBuilding(TSubclassOf<ABaseBuildingActor> BuildingClass)
{
	bIgnoreHighlighted = false;

	Server_SpawnBuilding(BuildingClass, GetMousePositionResult().ImpactPoint);
}

void ASpectatorPlayerController::Server_SpawnBuilding_Implementation(TSubclassOf<ABaseBuildingActor> SpawnClass, const FVector& Location)
{
	FActorSpawnParameters Param;
	Param.Owner = this;
	Param.Instigator = GetPawnOrSpectator();
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto const Building = GetWorld()->SpawnActor<ABaseBuildingActor>(SpawnClass, Location, FRotator::ZeroRotator, Param);
}


