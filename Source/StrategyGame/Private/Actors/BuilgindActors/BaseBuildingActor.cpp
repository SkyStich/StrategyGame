// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "Net/UnrealNetwork.h"
#include "NavigationSystem.h"
#include "PhysXInterfaceWrapperCore.h"
#include "Kismet/GameplayStatics.h"
#include "Player/UI/Building/BuildingSlotBase.h"
#include "Player/HUD/StrategyGameBaseHUD.h"
#include "AI/Pawns/Base/BaseAIPawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/PlayerController/SpectatorPlayerController.h"

// Sets default values
ABaseBuildingActor::ABaseBuildingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	NetUpdateFrequency = 1.f;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	BoxCollision->SetRelativeLocation(FVector(0.f, 0.f, 515.f));
	RootComponent = BoxCollision;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	StaticMeshComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABaseBuildingActor::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseBuildingActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseBuildingActor, OwnerPlayerController, COND_OwnerOnly);	
}


void ABaseBuildingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBuildingActor::SetOwnerController(ASpectatorPlayerController* Controller)
{
	OwnerPlayerController = Controller;	
}

void ABaseBuildingActor::SpawnPawn(TSubclassOf<ABaseAIPawn> PawnClass, const TArray<FResourcesData>& ResourcesNeedToBuy)
{
	Server_SpawnPawn(PawnClass, ResourcesNeedToBuy);
}

void ABaseBuildingActor::Server_SpawnPawn_Implementation(TSubclassOf<ABaseAIPawn> PawnClass, const TArray<FResourcesData>& ResourcesNeedToBuy)
{
	FNavLocation SpawnLocation;
	FVector const Ext = BoxCollision->GetScaledBoxExtent() * 1.2f;
	UNavigationSystemV1* NavigationSystemV1 = UNavigationSystemV1::GetCurrent(GetWorld());
	float const FindRadius = BoxCollision->GetScaledBoxExtent().X + BoxCollision->GetScaledBoxExtent().Y + 120.f;
	
	if(NavigationSystemV1 && NavigationSystemV1->GetRandomPointInNavigableRadius(GetActorLocation(), FindRadius, SpawnLocation) && OwnerPlayerController)
	{
		if(UKismetMathLibrary::IsPointInBox(SpawnLocation.Location, BoxCollision->GetComponentLocation(), Ext))
		{
			Server_SpawnPawn(PawnClass, ResourcesNeedToBuy);
			return;
		}
		
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Instigator = OwnerPlayerController->GetPawnOrSpectator();
		SpawnParameters.Owner = OwnerPlayerController;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ABaseAIPawn* SpawnPawn = GetWorld()->SpawnActor<ABaseAIPawn>(PawnClass, SpawnLocation.Location, FRotator::ZeroRotator, SpawnParameters);
		if(SpawnPawn)
		{
			SpawnPawn->SetTeam(OwnerPlayerController->FindObjectTeam_Implementation());
			SpawnPawn->FinishSpawning(FTransform(SpawnLocation.Location));
		}
	}
}

