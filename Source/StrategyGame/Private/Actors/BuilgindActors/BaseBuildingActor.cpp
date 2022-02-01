// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "Engine/DataTable.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "PhysXInterfaceWrapperCore.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Pawns/Base/BaseAIPawn.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/HUD/StrategyGameBaseHUD.h"
#include "Player/PlayerController/SpectatorPlayerController.h"
#include "Player/UI/SpawnProgress/SpawnProgressSlotBase.h"

// Sets default values
ABaseBuildingActor::ABaseBuildingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	NetUpdateFrequency = 1.f;
	InitialDestination = FVector::ZeroVector;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	BoxCollision->SetRelativeLocation(FVector(0.f, 0.f, 515.f));
	RootComponent = BoxCollision;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	StaticMeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FClassFinder<USpawnProgressSlotBase>SpawnProgressSlotFinder(TEXT("/Game/Blueprints/UI/SpawnProgress/W_SpawnProgressSlot"));
	if(SpawnProgressSlotFinder.Succeeded()) SpawnProgressSlot = SpawnProgressSlotFinder.Class;
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
	DOREPLIFETIME_CONDITION(ABaseBuildingActor, QueueOfSpawn, COND_OwnerOnly);
	DOREPLIFETIME(ABaseBuildingActor, OwnerTeam);
}

void ABaseBuildingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBuildingActor::SetOwnerController(ASpectatorPlayerController* Controller)
{
	OwnerPlayerController = Controller;	
}

void ABaseBuildingActor::SpawnPawn(UDataTable* PawnData, const FName& Id)
{
	if(!PawnData)
	{
		FString const ContextString = TEXT("ABaseBuildingActor::SpawnPawn");
		UE_LOG(LogPlayerController, Error, TEXT("Pawn data table is null: --%s"), *ContextString);
		return;
	}
	if(QueueOfSpawn.Num() >= 9) return;
	
	Server_SpawnPawn(PawnData, Id);
}

void ABaseBuildingActor::Server_SpawnPawn_Implementation(UDataTable* PawnData, const FName& Key)
{
	if(!PawnData)
	{
		FString const ContextString = TEXT("ABaseBuildingActor::Server_SpawnPawn");
		UE_LOG(LogPlayerController, Error, TEXT("Pawn data table is null: --%s"), *ContextString);
		return;
	}
	if(QueueOfSpawn.Num() >= 9) return;
	
	FVector const SpawnLocation = FindSpawnLocation();
	FVector const Ext = BoxCollision->GetScaledBoxExtent() * 1.2f;
	
	if(OwnerPlayerController)
	{
		if(UKismetMathLibrary::IsPointInBox(SpawnLocation, BoxCollision->GetComponentLocation(), Ext))
		{
			Server_SpawnPawn(PawnData, Key);
			return;
		}
		
		/** find row */
		FBaseSpawnPawnData* PawnDataTable = PawnData->FindRow<FBaseSpawnPawnData>(Key, *PawnData->GetName());
		if(!PawnDataTable) return;

		/** Generate key for queue array */
		FName const QueueKey = FName(FString(Key.ToString() + "_" + FString::FromInt(FMath::RandRange(0, 9999))));
		QueueOfSpawn.Add(FQueueData(QueueKey, *PawnDataTable));

		if(!GetWorld()->GetTimerManager().IsTimerActive(SpawnPawnHandle))
		{
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUObject(this, &ABaseBuildingActor::OnSpawnPawn, QueueKey, SpawnLocation, PawnDataTable->BuilderClass);
			GetWorld()->GetTimerManager().SetTimer(SpawnPawnHandle, TimerDelegate, PawnDataTable->TimeBeforeSpawn, false);
		}
	}
}

void ABaseBuildingActor::OnSpawnPawn(FName Key, FVector SpawnLocation, TAssetSubclassOf<ABaseAIPawn> SpawnClass)
{
	QueueOfSpawn.RemoveAllSwap([&](FQueueData Item) -> bool { return Item.Key == Key; });
	GetWorld()->GetTimerManager().ClearTimer(SpawnPawnHandle);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = OwnerPlayerController->GetPawnOrSpectator();
	SpawnParameters.Owner = OwnerPlayerController;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ABaseAIPawn* BaseSpawnPawn = GetWorld()->SpawnActor<ABaseAIPawn>(USyncLoadLibrary::SyncLoadClass(this, SpawnClass), SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
	if(BaseSpawnPawn)
	{
		BaseSpawnPawn->SetTeam(OwnerPlayerController->FindObjectTeam_Implementation());
		BaseSpawnPawn->FinishSpawning(FTransform(SpawnLocation));
	}
	RefreshQueue();
}

void ABaseBuildingActor::RefreshQueue()
{
	if(GetLocalRole() != ROLE_Authority) return;
	
	FVector const SpawnLocation = FindSpawnLocation();
	FVector const Ext = BoxCollision->GetScaledBoxExtent() * 1.2f;
	if(OwnerPlayerController)
	{		
		if(UKismetMathLibrary::IsPointInBox(SpawnLocation, BoxCollision->GetComponentLocation(), Ext))
		{
			RefreshQueue();
			return;
		}

		FQueueData TempData = FQueueData();
		for(auto ByArray : QueueOfSpawn)
		{
			if(!ByArray.Key.IsNone())
			{
				TempData = ByArray;
				break;
			}
		}
		
		if(!GetWorld()->GetTimerManager().IsTimerActive(SpawnPawnHandle))
		{
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUObject(this, &ABaseBuildingActor::OnSpawnPawn, TempData.Key, SpawnLocation, TempData.Value.BuilderClass);
			GetWorld()->GetTimerManager().SetTimer(SpawnPawnHandle, TimerDelegate, TempData.Value.TimeBeforeSpawn, false);
		}
	}
}

FVector ABaseBuildingActor::FindSpawnLocation()
{
	FNavLocation SpawnLocation;
	UNavigationSystemV1* NavigationSystemV1 = UNavigationSystemV1::GetCurrent(GetWorld());
	float const FindRadius = BoxCollision->GetScaledBoxExtent().X + BoxCollision->GetScaledBoxExtent().Y + 120.f;
	return (NavigationSystemV1 && NavigationSystemV1->GetRandomPointInNavigableRadius(GetActorLocation(), FindRadius, SpawnLocation))
	? SpawnLocation.Location : FVector();
}

void ABaseBuildingActor::GiveOrderToTargetPawn_Implementation(const FVector& LocationToMove, AActor* ActorToMove)
{
	InitialDestination = LocationToMove;
	DrawDebugSphere(GetWorld(), LocationToMove, 80, 12, FColor::Cyan, false, 2.f);
}

EObjectTeam ABaseBuildingActor::FindObjectTeam_Implementation()
{
	return OwnerTeam;
}

void ABaseBuildingActor::Server_ClearSpawnPawnHandle_Implementation(const FName& Key)
{
	if(QueueOfSpawn.Num() <= 0) return;

	auto const Reference = QueueOfSpawn.FindByPredicate([&](FQueueData Item) -> bool { return Item.Key == Key; });

	GetWorld()->GetTimerManager().ClearTimer(SpawnPawnHandle);
	QueueOfSpawn.RemoveAllSwap([&](FQueueData Item) -> bool { return Item.Key == Key; });
}

void ABaseBuildingActor::GenerateQueueSlots()
{
	if(QueueOfSpawn.Num() <= 0) return;
	UBaseMatchWidget* MainWidget = OwnerPlayerController->GetHUD<AStrategyGameBaseHUD>()->GetMainWidget();
	MainWidget->ClearQueueBorder();
	int32 Index = -1;
	for(const auto& ByArray : QueueOfSpawn)
	{
		Index++;
		auto SlotWidget = CreateWidget<USpawnProgressSlotBase>(OwnerPlayerController, SpawnProgressSlot);
		if(SlotWidget)
		{
			SlotWidget->SetSpawnTime(ByArray.Value.TimeBeforeSpawn);
			SlotWidget->SetId(ByArray.Key);
			SlotWidget->SetBuildOwner(this);
			SlotWidget->SetIcon(USyncLoadLibrary::SyncLoadTexture(this, ByArray.Value.Icon));
			MainWidget->AttachSlotToQueue(SlotWidget);
		}
	}
}

void ABaseBuildingActor::HighlightedActor_Implementation(AStrategyGameBaseHUD* PlayerHUD)
{
	if(GetLocalRole() != ROLE_Authority || GetNetMode() == NM_Standalone || GetNetMode() == NM_ListenServer)
	GenerateQueueSlots();
}

void ABaseBuildingActor::GenerateQueueSlot(const FName& Id, const FBaseSpawnPawnData& SpawnData)
{
	UBaseMatchWidget* MainWidget = OwnerPlayerController->GetHUD<AStrategyGameBaseHUD>()->GetMainWidget();
	auto SlotWidget = CreateWidget<USpawnProgressSlotBase>(OwnerPlayerController, SpawnProgressSlot);
	if(SlotWidget)
	{
		SlotWidget->SetSpawnTime(SpawnData.TimeBeforeSpawn);
		SlotWidget->SetId(Id);
		SlotWidget->SetIcon(USyncLoadLibrary::SyncLoadTexture(this, SpawnData.Icon));
		SlotWidget->SetBuildOwner(this);
		MainWidget->AttachSlotToQueue(SlotWidget);
	}
}

void ABaseBuildingActor::RemoveSlotFromQueue(USpawnProgressSlotBase* SlotForRemove)
{
	QueueOfSpawn.RemoveAllSwap([&](FQueueData Item) -> bool { return Item.Key == SlotForRemove->GetId(); });
	GenerateQueueSlots();
	Server_RemoveItemFromQueue(SlotForRemove->GetId());
}

void ABaseBuildingActor::Server_RemoveItemFromQueue_Implementation(const FName& Id)
{
	QueueOfSpawn.RemoveAllSwap([&](FQueueData Item) -> bool { return Item.Key == Id; });
}


