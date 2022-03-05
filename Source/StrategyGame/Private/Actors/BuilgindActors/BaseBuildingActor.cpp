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
#include "GameFramework/PlayerState.h"
#include "GameInstance/Subsystems/GameAIPawnSubsystem.h"

// Sets default values
ABaseBuildingActor::ABaseBuildingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	NetUpdateFrequency = 1.f;
	bIsHighlighted = false;
	InitialDestination = FVector::ZeroVector;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	BoxCollision->SetRelativeLocation(FVector(0.f, 0.f, 515.f));
	RootComponent = BoxCollision;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	StaticMeshComponent->SetupAttachment(RootComponent);
	
	ObjectHealthComponent = CreateDefaultSubobject<UObjectHealthComponent>(TEXT("ObjectHealthComponent"));

	static ConstructorHelpers::FClassFinder<USpawnProgressSlotBase>SpawnProgressSlotFinder(TEXT("/Game/Blueprints/UI/SpawnProgress/W_SpawnProgressSlot"));
	if(SpawnProgressSlotFinder.Succeeded()) SpawnProgressSlot = SpawnProgressSlotFinder.Class;
}

// Called when the game starts or when spawned
void ABaseBuildingActor::BeginPlay()
{
	Super::BeginPlay();
	
	if(OwnerPlayerController && GetLocalRole() != ROLE_Authority)
	{
		OwnerPlayerController->OnActionWithObjectReleasedEvent.AddDynamic(this, &ABaseBuildingActor::UnHighlighted);
	}
}

void ABaseBuildingActor::UnHighlighted()
{
	bIsHighlighted = false;
}

void ABaseBuildingActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseBuildingActor, OwnerPlayerController, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABaseBuildingActor, QueueOfSpawn, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABaseBuildingActor, BuildName, COND_InitialOnly);
	DOREPLIFETIME(ABaseBuildingActor, OwnerTeam);
}

void ABaseBuildingActor::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
}

void ABaseBuildingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseBuildingActor::SetOwnerController(ASpectatorPlayerController* Controller)
{
	OwnerPlayerController = Controller;	
}

void ABaseBuildingActor::StartSpawnTimer(const FName& Key)
{
	if(GetLocalRole() != ROLE_Authority && !GetWorld()->GetTimerManager().IsTimerActive(SpawnPawnHandle))
	{
		auto const Ping = OwnerPlayerController->PlayerState->GetPing() / 10;
		auto const PawnData = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>()->GetPawnDataByTeam(OwnerTeam);
		if(!PawnData) return;
		
		auto const RowData = PawnData->FindRow<FAIPawnData>(Key, *PawnData->GetName());
		if(!RowData) return;

		FTimerDelegate TimerDel;
		TimerDel.BindUObject(this, &ABaseBuildingActor::OnSpawnComplete);
		GetWorld()->GetTimerManager().SetTimer(SpawnPawnHandle, TimerDel, RowData->TimeBeforeSpawn + Ping, false);
	}
}

void ABaseBuildingActor::RefreshSpawnTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnPawnHandle);
	if(QueueOfSpawn.Num() <= 0) return;
	
	auto const Ping = OwnerPlayerController->PlayerState->GetPing() / 10;
	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &ABaseBuildingActor::OnSpawnComplete);
	FQueueData TempData = FQueueData();
	for(auto ByArray : QueueOfSpawn)
	{
		if(!ByArray.QueueId.IsNone())
		{
			TempData = ByArray;
			break;
		}
	}
	GetWorld()->GetTimerManager().SetTimer(SpawnPawnHandle, TimerDel, TempData.Value.TimeBeforeSpawn + Ping, false);
}

void ABaseBuildingActor::OnSpawnComplete()
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnPawnHandle);
}

void ABaseBuildingActor::SpawnPawn(const FName& Id)
{
	if(QueueOfSpawn.Num() >= 9) return;	
	Server_SpawnPawn(Id);
	StartSpawnTimer(Id);
}

void ABaseBuildingActor::Server_SpawnPawn_Implementation(const FName& Key)
{
	StartSpawnPawn(Key);
}

void ABaseBuildingActor::StartSpawnPawn(const FName& Key)
{
	if(GetLocalRole() != ROLE_Authority || QueueOfSpawn.Num() >= 9) return;
	
	FVector const SpawnLocation = FindSpawnLocation();
	FVector const Ext = BoxCollision->GetScaledBoxExtent() * 1.2f;
	
	if(OwnerPlayerController)
	{
		auto const PawnData = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>()->GetPawnDataByTeam(OwnerTeam);
		
		if(!PawnData)
		{
			/** @todo add logic for bug fixed */
			return;
		}
		
		/** find row */
		FAIPawnData* PawnDataTable = PawnData->FindRow<FAIPawnData>(Key, *PawnData->GetName());
		if(!PawnDataTable) return;
		/** Generate key for queue array */
		FName QueueKey = FName(FString(Key.ToString() + "_" + FString::FromInt(FMath::RandRange(0, 9999))));
		if(QueueOfSpawn.FindByPredicate([&](FQueueData Item) -> bool { return Item.QueueId == QueueKey; }))
		{
			StartSpawnPawn(Key);
			return;
		}

		FQueueData const QueueData(FQueueData(QueueKey, *PawnDataTable));
		QueueOfSpawn.Add(QueueData);

		if(GetWorld()->GetTimerManager().IsTimerActive(SpawnPawnHandle)) return;
		if(UKismetMathLibrary::IsPointInBox(SpawnLocation, BoxCollision->GetComponentLocation(), Ext))
		{
			StartSpawnPawn(Key);
			return;
		}
		
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &ABaseBuildingActor::OnSpawnPawn, QueueData, SpawnLocation);
		GetWorld()->GetTimerManager().SetTimer(SpawnPawnHandle, TimerDelegate, PawnDataTable->TimeBeforeSpawn, false);
	}
}

void ABaseBuildingActor::OnSpawnPawn(FQueueData QueueData, FVector SpawnLocation)
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnPawnHandle);
	QueueOfSpawn.RemoveAll([&](FQueueData Item) -> bool { return Item.QueueId == QueueData.QueueId; });
	Client_OnSpawnFinished(QueueData.QueueId);
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Instigator = OwnerPlayerController->GetPawnOrSpectator();
	SpawnParameters.Owner = OwnerPlayerController;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ABaseAIPawn* BaseSpawnPawn = GetWorld()->SpawnActor<ABaseAIPawn>(USyncLoadLibrary::SyncLoadClass(this, QueueData.Value.PawnClass), SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
	if(BaseSpawnPawn)
	{
		BaseSpawnPawn->SetTeam(OwnerPlayerController->FindObjectTeam_Implementation());
		BaseSpawnPawn->InitPawn(QueueData.Value);
		BaseSpawnPawn->FinishSpawning(FTransform(SpawnLocation));
	}
	RefreshQueue();
}

void ABaseBuildingActor::Client_OnSpawnFinished_Implementation(const FName& Key)
{
	QueueOfSpawn.RemoveAll([&](FQueueData Item) -> bool { return Item.QueueId == Key; });
	RefreshSpawnTimer();
	GenerateQueueSlots();
}

void ABaseBuildingActor::RefreshQueue()
{
	if(GetLocalRole() != ROLE_Authority) return;

	if(QueueOfSpawn.Num() <= 0) return;
	
	GetWorld()->GetTimerManager().ClearTimer(SpawnPawnHandle);
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
			if(!ByArray.QueueId.IsNone())
			{
				TempData = ByArray;
				break;
			}
		}
		
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &ABaseBuildingActor::OnSpawnPawn, TempData, SpawnLocation);
		GetWorld()->GetTimerManager().SetTimer(SpawnPawnHandle, TimerDelegate, TempData.Value.TimeBeforeSpawn, false);
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
	DrawDebugSphere(GetWorld(), LocationToMove,1200, 12, FColor::Cyan, false, 2.f);
}

EObjectTeam ABaseBuildingActor::FindObjectTeam_Implementation()
{
	return OwnerTeam;
}

void ABaseBuildingActor::Server_ClearSpawnPawnHandle_Implementation(const FName& Key)
{
	if(QueueOfSpawn.Num() <= 0) return;

	auto const Reference = QueueOfSpawn.FindByPredicate([&](FQueueData Item) -> bool { return Item.QueueId == Key; });

	GetWorld()->GetTimerManager().ClearTimer(SpawnPawnHandle);
	QueueOfSpawn.RemoveAllSwap([&](FQueueData Item) -> bool { return Item.QueueId == Key; });
}

void ABaseBuildingActor::GenerateQueueSlots()
{
	UBaseMatchWidget* MainWidget = OwnerPlayerController->GetHUD<AStrategyGameBaseHUD>()->GetMainWidget();
	MainWidget->ClearQueueBorder();
	int32 Index = -1;
	for(const auto& ByArray : QueueOfSpawn)
	{
		if(ByArray.QueueId.IsNone()) continue;
		
		Index++;
		float TempMaxTime = ByArray.Value.TimeBeforeSpawn;
		if(Index == 0 && GetWorld()->GetTimerManager().IsTimerActive(SpawnPawnHandle))
		{
			TempMaxTime = GetWorld()->GetTimerManager().GetTimerRemaining(SpawnPawnHandle);
		}
		
		auto SlotWidget = CreateWidget<USpawnProgressSlotBase>(OwnerPlayerController, SpawnProgressSlot);
		if(SlotWidget)
		{
			SlotWidget->SetSpawnTime(ByArray.Value.TimeBeforeSpawn);
			SlotWidget->SetId(ByArray.QueueId);
			SlotWidget->SetBuildOwner(this);
			SlotWidget->SetIcon(USyncLoadLibrary::SyncLoadTexture(this, ByArray.Value.Icon));
			MainWidget->AttachSlotToQueue(SlotWidget);
		}
	}
}

void ABaseBuildingActor::HighlightedActor_Implementation(AStrategyGameBaseHUD* PlayerHUD)
{
	if(!ObjectHealthComponent->IsAlive()) return;
	Server_Highlighted();
	UDataTable* TempSpawnData = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>()->GetPawnDataByTeam(OwnerTeam);
	if(!TempSpawnData)
	{
		/** @todo add logic for bug fixed */
		return;
	}
		
	TArray<UActionBaseSlot*> SpawnSlots;
	for(auto ByArray : PawnRowNames)
	{
		FString const ContextString = TempSpawnData->GetName();
		FAIPawnData* PawnData = TempSpawnData->FindRow<FAIPawnData>(ByArray, ContextString);
		if(!PawnData)
		{
			UE_LOG(LogActor, Error, TEXT("PawnRowNames have not valid row! --%s"), *GetFullName());
			continue;
		}
			
		auto TempSlot = USyncLoadLibrary::SyncLoadWidget<UActionSpawnPawnSlotBase>(this, PlayerHUD->GetActionSpawnPawnSlotClass(), OwnerPlayerController);
		if(TempSlot)
		{
			TempSlot->Init(this, ByArray, PawnData->Icon);
			SpawnSlots.Add(TempSlot);
		}
	}
	PlayerHUD->CreateActionGrid(SpawnSlots);
	GenerateQueueSlots();
}

void ABaseBuildingActor::GenerateQueueSlot(const FName& Id)
{
	UBaseMatchWidget* MainWidget = OwnerPlayerController->GetHUD<AStrategyGameBaseHUD>()->GetMainWidget();
	
	UDataTable* TempSpawnData = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>()->GetPawnDataByTeam(OwnerTeam);
	if(!TempSpawnData)
	{ 
		/** @todo add logic for bug fixed */
		return;
	}
	FString const ContextString = TempSpawnData->GetName();
	FAIPawnData* SpawnData = TempSpawnData->FindRow<FAIPawnData>(Id, ContextString);
	
	auto SlotWidget = CreateWidget<USpawnProgressSlotBase>(OwnerPlayerController, SpawnProgressSlot);
	if(SlotWidget)
	{
		SlotWidget->SetSpawnTime(SpawnData->TimeBeforeSpawn);
		SlotWidget->SetId(Id);
		SlotWidget->SetIcon(USyncLoadLibrary::SyncLoadTexture(this, SpawnData->Icon));
		SlotWidget->SetBuildOwner(this);
		MainWidget->AttachSlotToQueue(SlotWidget);
	}
}

void ABaseBuildingActor::RemoveSlotFromQueue(USpawnProgressSlotBase* SlotForRemove)
{
	int32 const RemoveIndex = QueueOfSpawn.IndexOfByPredicate([&](FQueueData Item) -> bool { return Item.QueueId == SlotForRemove->GetId(); });
	bool const IsFrontSlot = !QueueOfSpawn.IsValidIndex(RemoveIndex - 1);
	QueueOfSpawn.RemoveAll([&](FQueueData Item) -> bool { return Item.QueueId == SlotForRemove->GetId(); });
	if(IsFrontSlot)
	{
		RefreshSpawnTimer();
	}
	GenerateQueueSlots();
	Server_RemoveItemFromQueue(SlotForRemove->GetId());
}

void ABaseBuildingActor::Server_RemoveItemFromQueue_Implementation(const FName& Id)
{
	int32 const RemoveIndex = QueueOfSpawn.IndexOfByPredicate([&](FQueueData Item) -> bool { return Item.QueueId == Id; });
	if(!QueueOfSpawn.IsValidIndex(RemoveIndex - 1))
	{
		QueueOfSpawn.RemoveAll([&](FQueueData Item) -> bool { return Item.QueueId == Id; });
		RefreshQueue();
		return;
	}
	QueueOfSpawn.RemoveAll([&](FQueueData Item) -> bool { return Item.QueueId == Id; });
}

void ABaseBuildingActor::Server_Highlighted_Implementation()
{
	bIsHighlighted = true;
}

void ABaseBuildingActor::Server_UnHighlighted_Implementation()
{
	UnHighlighted();
}