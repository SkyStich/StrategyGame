// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerController/SpectatorPlayerController.h"
#include "CollisionDebugDrawingPublic.h"
#include "Net/UnrealNetwork.h"
#include "Actors/StaticMeshActors/PreBuildingActor.h"
#include "AI/Controllers/Base/BaseBuilderAIController.h"
#include "AI/Pawns/Base/BaseAIPawn.h"
#include "Engine/ActorChannel.h"
#include "Engine/PostProcessVolume.h"
#include "GameInstance/Subsystems/GameAIPawnSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Components/SpectatorCameraComponent.h"
#include "Player/Interfaces/HighlightedInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Player/HUD/StrategyGameBaseHUD.h"
#include "Player/PlayerStates/StrategyMatchPlayerState.h"
#include "Player/Objects/PlayerImprovementCharacteristics.h"

ASpectatorPlayerController::ASpectatorPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	bShowMouseCursor =  true;
	bIgnoreHighlighted = false;
	bReplicates = true;
	NetUpdateFrequency = 10.f;
	HighlightedTag = "IsTarget";
	ResourcesActorComponent = CreateDefaultSubobject<UResourcesActorComponent>(TEXT("ResourcesActorComponent"));

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> HighlightedInstanceFinder(TEXT("/Game/Assets/SelectingObject/Materials/PP_Outliner_Inst"));
	if(HighlightedInstanceFinder.Succeeded()) HighlightedMaterialInstance = HighlightedInstanceFinder.Object;	
}

void ASpectatorPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if(GetLocalRole() != ROLE_Authority || GetNetMode() == NM_Standalone)
	{
		SetInputMode(FInputModeGameOnly());
		SpawnPostProcess();
	}
}

bool ASpectatorPlayerController::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	return Super::ReplicateSubobjects(Channel, Bunch, RepFlags);
}

void ASpectatorPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ASpectatorPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(GetLocalRole() != ROLE_Authority || GetNetMode() == NM_Standalone || GetNetMode() == NM_ListenServer)
	{
		UpdateHighlightedActor();
	}
}

void ASpectatorPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASpectatorPlayerController, TargetActors, COND_OwnerOnly);
}

void ASpectatorPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	check(InputComponent);

	InputComponent->BindAction("ActionWithObject", IE_Pressed, this, &ASpectatorPlayerController::OnActionWithObjectPressed);
	InputComponent->BindAction("ActionWithObject", IE_Released, this, &ASpectatorPlayerController::OnSelectActorReleased);

	InputComponent->BindAction("OrderToTarget", IE_Released, this, &ASpectatorPlayerController::MoveTargetPawnsPressed);
	InputComponent->BindAction("TEST", IE_Released, this, &ASpectatorPlayerController::Test);
}

void ASpectatorPlayerController::Test()
{
	TArray<AActor*> Out;
	UGameplayStatics::GetAllActorsOfClass(this, ABaseAIPawn::StaticClass(), Out);
	for(auto ByArray : Out)
	{
		if(ByArray)
		{
			Cast<ABaseAIController>(Cast<APawn>(ByArray)->Controller)->ToggleAggressiveType();
			return;
		}
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

FVector2D ASpectatorPlayerController::GetMousePositionCustom() const
{
	float X = 0.f;
	float Y = 0.f;
	GetMousePosition(X, Y);
	return FVector2D(X, Y);
}

void ASpectatorPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	if(GetLocalRole() != ROLE_Authority || GetNetMode() == NM_Standalone || GetNetMode() == NM_ListenServer) OnNewPawn.Broadcast(GetPawn());
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
		if(MousePositionResult.GetActor()->Tags.Contains(HighlightedTag)) return;
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
			return;
		}
	}
	
	/** if the object is no longer selected, clear the static variable and turn it off custom depth  */
	if(IsValid(HighlightedActor))
	{
		if(!HighlightedActor->Tags.Contains(HighlightedTag)) UpdateCustomDepthFromActor(HighlightedActor, false);
		HighlightedActor = nullptr;	
	}
}

void ASpectatorPlayerController::UpdateCustomDepthFromActor(AActor* Actor, bool bState)
{	
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

void ASpectatorPlayerController::AddSingleTargetActor(AActor* NewTarget)
{
	if(GetLocalRole() != ROLE_Authority) return;
	if(TargetActors.Contains(NewTarget) || !NewTarget->GetClass()->ImplementsInterface(UHighlightedInterface::StaticClass())) return;
	
	if(NewTarget)
	{
		TargetActors.Add(NewTarget);
		BindOnTargetActorDeath(NewTarget);
	}
	
	TargetActorUpdated.Broadcast(NewTarget);
}

void ASpectatorPlayerController::AddTargetPawns(const TArray<ABaseAIPawn*>& NewTargets)
{
	if(TargetActors.Num() > 0)
	{
		for(const auto& ByArray : TargetActors)
		{
			ByArray->Tags.Remove(HighlightedTag);
			UnBindTargetActorDeath(ByArray);
			UpdateCustomDepthFromActor(ByArray, false);
		}
		
		for(const auto& ByArray : NewTargets)
		{
			UpdateCustomDepthFromActor(ByArray, true);
		}
	}
	Server_AddTargetPawns(NewTargets);
}

void ASpectatorPlayerController::Server_AddTargetPawns_Implementation(const TArray<ABaseAIPawn*>& NewTargets)
{
	ClearTargetActors();

	if(NewTargets.Num() <= 0) return;

	for(auto& ByArray : NewTargets)
	{
		AddSingleTargetActor(ByArray);
	}
}

void ASpectatorPlayerController::ClearTargetActors()
{
	if(GetLocalRole() != ROLE_Authority) return;
		
	for(const auto& ByArray : TargetActors)
	{
		RemoveActorFromTarget(ByArray);
	}
}

void ASpectatorPlayerController::RemoveActorFromTarget(AActor* Actor)
{
	if(GetLocalRole() != ROLE_Authority) return;
	UnBindTargetActorDeath(Actor);
	TargetActors.Remove(Actor);
}

void ASpectatorPlayerController::OnSelectActorReleased()
{
	auto StrategyHUD = GetStrategyGameBaseHUD();
	StrategyHUD->HiddenHealthStatistics();
	
	FHitResult OutHit;
	ETraceTypeQuery const TraceChannel = UCollisionProfile::Get()->ConvertToTraceType(bIgnoreHighlighted ? ECC_Camera : ECC_GameTraceChannel1);
	
	if(GetHitResultUnderCursorByChannel(TraceChannel, true, OutHit) && OutHit.GetActor())
	{
		auto const TempOutActorClass = OutHit.GetActor()->GetClass();
		if(TempOutActorClass->ImplementsInterface(UHighlightedInterface::StaticClass()) && TempOutActorClass->ImplementsInterface(UFindObjectTeamInterface::StaticClass()))
		{
			if(!TargetActors.Contains(OutHit.GetActor()))
			{
				/** remove custom depth on old target actors */
				if(TargetActors.Num() > 0)
				{
					for(const auto& ByArray :  TargetActors)
					{
						ByArray->Tags.Remove(HighlightedTag);
						UpdateCustomDepthFromActor(ByArray, false);
						UnBindTargetActorDeath(ByArray);
					}
				}
				/** return if actor can not be damage. (m.b. Actor is dead) */
				if(!OutHit.GetActor()->CanBeDamaged()) return;
				
				OutHit.GetActor()->Tags.Add(HighlightedTag);
				BindOnTargetActorDeath(OutHit.GetActor());

				Server_SingleSelectActor(OutHit.TraceStart, OutHit.TraceEnd);

				/** if hit actor != local player team show custom depth and health statistics */
				if(Execute_FindObjectTeam(OutHit.GetActor()) != GetStrategyPlayerState()->GetPlayerTeam())
				{
					UpdateCustomDepthFromActor(OutHit.GetActor(), true);
					StrategyHUD->ShowHealthStatistics(OutHit.GetActor());
				}
			}
		}
	}
	else
	{
		if(!bIgnoreHighlighted)
		{
			StrategyHUD->ClearActionGrid();
			if(TargetActors.Num() > 0)
			{
				ClearTargetAllActorsDepth();
				Server_ClearTargetActors();
			}
		}
	}
	OnActionWithObjectReleasedEvent.Broadcast();
}

void ASpectatorPlayerController::Server_ClearTargetActors_Implementation()
{
	ClearTargetActors();
}

void ASpectatorPlayerController::Server_SingleSelectActor_Implementation(const FVector& TraceStart, const FVector& TraceEnd)
{
	FHitResult OutHit;
	ECollisionChannel const TraceChannel = bIgnoreHighlighted ? ECC_Camera : ECC_GameTraceChannel1;
	if(!GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, TraceChannel)) return;
	
	if(OutHit.GetActor())
	{
		auto const TempOutClass = OutHit.GetActor()->GetClass();
		if(!TempOutClass->ImplementsInterface(UFindObjectTeamInterface::StaticClass()) || !TempOutClass->ImplementsInterface(UHighlightedInterface::StaticClass()))
		{
			return;
		}
		
		if(!TargetActors.Contains(OutHit.GetActor()))
		{
			if(TargetActors.Num() > 0)
			{
				ClearTargetActors();
			}
			AddSingleTargetActor(OutHit.GetActor());

			if(Execute_FindObjectTeam(OutHit.GetActor()) == GetStrategyPlayerState()->GetPlayerTeam())
				Client_CallHighlightedOnSelectObject(OutHit.GetActor());
		}
	}
}

void ASpectatorPlayerController::Client_CallHighlightedOnSelectObject_Implementation(AActor* Target)
{
	UpdateCustomDepthFromActor(Target, true);
	IHighlightedInterface::Execute_HighlightedActor(Target, GetStrategyGameBaseHUD());
	auto const TempHUD = GetStrategyGameBaseHUD();
	if(TempHUD) TempHUD->ShowHealthStatistics(Target);
}

void ASpectatorPlayerController::ClearTargetAllActorsDepth()
{
	if(TargetActors.Num() > 0)
	{
		for(const auto& ByArray : TargetActors)
		{
			ByArray->Tags.Remove(HighlightedTag);
			UnBindTargetActorDeath(ByArray);
			UpdateCustomDepthFromActor(ByArray, false);
		}
	}
}

void ASpectatorPlayerController::OnActionWithObjectPressed()
{
	auto StrategyHUD = GetStrategyGameBaseHUD();
	StrategyHUD->SetGroupSelectionActive(true);
	StrategyHUD->StartGroupSelectionPosition = GetMousePositionCustom();
}

void ASpectatorPlayerController::SpawnBuilding(TSubclassOf<ABaseBuildingActor> BuildingClass, const FName& RowName)
{
	bIgnoreHighlighted = false;

	Server_SpawnBuilding(BuildingClass, GetMousePositionResult().ImpactPoint, RowName);
}

void ASpectatorPlayerController::Server_SpawnBuilding_Implementation(TSubclassOf<ABaseBuildingActor> SpawnClass, const FVector& Location, const FName& RowName)
{
	/** check on have valid reosurces */
	FBaseBuilderData* BuilderData = nullptr;
	auto const Subsystem = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>();
	auto const BuilderDataTable = Subsystem->GetBuilderDataByTeam(GetStrategyPlayerState()->GetPlayerTeam());
	if(BuilderDataTable)
	{
		FString const ContextString = FString("DT_BuilderData");
		BuilderData = BuilderDataTable->FindRow<FBaseBuilderData>(RowName, ContextString);
		if(BuilderData == nullptr) return;

		if(!ResourcesActorComponent->EnoughResources(BuilderData->ResourcesNeedToBuy)) return;	
	}

	for(const auto& ByArray : BuilderData->ResourcesNeedToBuy)
	{
		ResourcesActorComponent->DecreaseResourcesValue(ByArray.Key, ByArray.Value);
	}
	
	FActorSpawnParameters Param;
	Param.Owner = this;
	Param.Instigator = GetPawnOrSpectator();
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto const Building = GetWorld()->SpawnActor<ABaseBuildingActor>(SpawnClass, Location, FRotator::ZeroRotator, Param);

	if(Building)
	{
		Building->SetOwnerController(this);
		Building->SetTeamOwner(GetStrategyPlayerState()->GetPlayerTeam());
		Building->SetBuildName(BuilderData->DisplayName);
		Building->GetObjectHealthComponent()->CalculateBuildConstructHealth(BuilderData->MaxHealth);
		Building->FinishSpawning(FTransform(Location));
	}
}

void ASpectatorPlayerController::AddResourcesByType(const EResourcesType Type, const int32 Value)
{
	ResourcesActorComponent->IncreaseResourcesValue(Type, abs(Value));
	ForceNetUpdate();
}

void ASpectatorPlayerController::DecreaseResourcesByType(const EResourcesType Type, const int32 Value)
{
	ResourcesActorComponent->DecreaseResourcesValue(Type, abs(Value));
	ForceNetUpdate();
}

void ASpectatorPlayerController::OnActionTargetPawn()
{
	OnActionTargetActor.Broadcast();
	if(GetNetMode() != NM_Standalone) Server_ActionTargetPawn();
}

void ASpectatorPlayerController::Server_ActionTargetPawn_Implementation()
{
	OnActionTargetActor.Broadcast();
}

AStrategyMatchPlayerState* ASpectatorPlayerController::GetStrategyPlayerState() const
{
	auto const PS = Cast<AStrategyMatchPlayerState>(PlayerState);
	return PS;
}


EObjectTeam ASpectatorPlayerController::FindObjectTeam_Implementation()
{
	auto const PS = GetStrategyPlayerState();
	return PS ? PS->GetPlayerTeam() : EObjectTeam::None;
}

void ASpectatorPlayerController::MoveTargetPawnsPressed()
{
	if(TargetActors.Num() <= 0) return;

	/** check on team */
	for(const auto& ByArray : TargetActors)
	{
		if(!ByArray->GetClass()->ImplementsInterface(UFindObjectTeamInterface::StaticClass())) return;
		if(Execute_FindObjectTeam(ByArray) != FindObjectTeam_Implementation()) return;
	}
	
	FHitResult OutHit;
	ETraceTypeQuery const TraceChannel = UCollisionProfile::Get()->ConvertToTraceType(bIgnoreHighlighted ? ECC_Camera : ECC_GameTraceChannel1);
	GetHitResultUnderCursorByChannel(TraceChannel, true, OutHit);
	
	for(const auto& ByArray : TargetActors)
	{
		if(!ByArray->Tags.Contains(HighlightedTag)) continue;
		
		if(ByArray->GetClass()->ImplementsInterface(UGiveOrderToTargetPawns::StaticClass()))
		{
			IGiveOrderToTargetPawns::Execute_GiveOrderToTargetPawn(ByArray, OutHit.ImpactPoint, OutHit.GetActor());
		}
	}
	if(GetNetMode() != NM_Standalone) Server_MoveTargetPawns(OutHit.TraceStart, OutHit.TraceEnd);
}

void ASpectatorPlayerController::Server_MoveTargetPawns_Implementation(const FVector& TraceStart, const FVector& TraceEnd)
{
	if(TargetActors.Num() <= 0) return;

	/** check on team */
	for(const auto& ByArray : TargetActors)
	{
		if(!ByArray->GetClass()->ImplementsInterface(UFindObjectTeamInterface::StaticClass())) return;
		if(Execute_FindObjectTeam(ByArray) != FindObjectTeam_Implementation()) return;
	}
	
	FHitResult OutHit;
	ECollisionChannel const TraceChannel = bIgnoreHighlighted ? ECC_Camera : ECC_GameTraceChannel1;
	GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, TraceChannel);
	for(const auto& ByArray : TargetActors)
	{
		if(ByArray->GetClass()->ImplementsInterface(UGiveOrderToTargetPawns::StaticClass()))
		{
			IGiveOrderToTargetPawns::Execute_GiveOrderToTargetPawn(ByArray, OutHit.ImpactPoint, OutHit.GetActor());
		}
	}
}

void ASpectatorPlayerController::SpawnPreBuildAction(TSubclassOf<ABaseBuildingActor> BuildActor, const FName& RowName)
{
	if(!BuildActor) return;
	
	auto const SubobjectMesh = Cast<UStaticMeshComponent>(BuildActor->GetDefaultSubobjectByName("StaticMesh"));
	auto const SubobjectBox = Cast<UBoxComponent>(BuildActor->GetDefaultSubobjectByName("BoxComponent"));
	if(SubobjectMesh && SubobjectBox)
	{
		APreBuildingActor* PreBuildingActor = nullptr;
		FActorSpawnParameters Param;
		Param.Owner = this;
		Param.Instigator = GetPawnOrSpectator();
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		PreBuildingActor = GetWorld()->SpawnActor<APreBuildingActor>(APreBuildingActor::StaticClass(), Param);
		if(PreBuildingActor)
		{
			bIgnoreHighlighted = true;		
			PreBuildingActor->GetStaticMeshComponent()->SetStaticMesh(SubobjectMesh->GetStaticMesh());
			PreBuildingActor->GetStaticMeshComponent()->SetRelativeScale3D(SubobjectMesh->GetRelativeScale3D());
			PreBuildingActor->SetBoxExtent(SubobjectBox->GetScaledBoxExtent());
			PreBuildingActor->SetOwnerController(this);
			PreBuildingActor->SetBuildRowName(RowName);
			PreBuildingActor->SetBuildingActor(BuildActor);
				
			PreBuildingActor->FinishSpawning(FTransform(FVector(0.f)));
		}
	}
}

void ASpectatorPlayerController::OnTargetDeath(AActor* Actor)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		RemoveActorFromTarget(Actor);
		return;
	}
	UpdateCustomDepthFromActor(Actor, false);
	Actor->Tags.Remove(HighlightedTag);
	auto StrategyHUD = GetStrategyGameBaseHUD();
	if(!StrategyHUD) return;
	
	if(TargetActors.Num() <= 1)
	{
		StrategyHUD->ClearActionGrid();
		StrategyHUD->HiddenHealthStatistics();
	}
}

void ASpectatorPlayerController::BindOnTargetActorDeath(AActor* Target)
{
	if(!Target) return;
	UObjectHealthComponent* HealthComponent = Target->FindComponentByClass<UObjectHealthComponent>();
	if(HealthComponent) HealthComponent->OnHealthEnded.AddDynamic(this, &ASpectatorPlayerController::OnTargetDeath);
}

void ASpectatorPlayerController::UnBindTargetActorDeath(AActor* Target)
{
	if(!Target) return;
	UObjectHealthComponent* HealthComponent = Target->FindComponentByClass<UObjectHealthComponent>();
	if(HealthComponent) HealthComponent->OnHealthEnded.Remove(this, "OnTargetDeath");
}