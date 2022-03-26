// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "Engine/DataTable.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Pawns/Base/BaseAIPawn.h"
#include "NavigationSystem.h"
#include "BlueprintFunctionLibraries/AsyncLoadLibrary.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/HUD/StrategyGameBaseHUD.h"
#include "Player/PlayerController/SpectatorPlayerController.h"
#include "Player/UI/SpawnProgress/SpawnProgressSlotBase.h"
#include "GameFramework/PlayerState.h"
#include "GameInstance/Subsystems/GameAIPawnSubsystem.h"
#include "Player/UI/ActionSlots/Base/ImprovementSlotBase.h"
#include "Player/UI/SpawnProgress/BuildingSpawnProgressSlotBase.h"

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
	DOREPLIFETIME_CONDITION(ABaseBuildingActor, ImprovementQueue, COND_OwnerOnly);
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
	if(GetLocalRole() != ROLE_Authority && !GetWorld()->GetTimerManager().IsTimerActive(ProgressTimerHandle))
	{
		auto const Ping = OwnerPlayerController->PlayerState->GetPing() / 10;
		auto const PawnData = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>()->GetPawnDataByTeam(OwnerTeam);
		if(!PawnData) return;
		
		auto const RowData = PawnData->FindRow<FAIPawnData>(Key, *PawnData->GetName());
		if(!RowData) return;

		FTimerDelegate TimerDel;
		TimerDel.BindUObject(this, &ABaseBuildingActor::OnSpawnComplete);
		GetWorld()->GetTimerManager().SetTimer(ProgressTimerHandle, TimerDel, RowData->TimeBeforeSpawn + Ping, false);
	}
}

void ABaseBuildingActor::RefreshSpawnTimer()
{
	GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
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
	GetWorld()->GetTimerManager().SetTimer(ProgressTimerHandle, TimerDel, TempData.Value.TimeBeforeSpawn + Ping, false);
}

void ABaseBuildingActor::OnSpawnComplete()
{
	GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
}

void ABaseBuildingActor::SpawnPawn(const FName& Id)
{
	if(QueueOfSpawn.Num() >= 9 - ImprovementQueue.Num() || !GetOwnerController()) return;
	if(ImprovementQueue.Num() > 0) return;
	
	Server_SpawnPawn(Id);
	StartSpawnTimer(Id);
}

void ABaseBuildingActor::Server_SpawnPawn_Implementation(const FName& Key)
{
	if(ImprovementQueue.Num() > 0) return;
	
	auto const AISubsystem = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>();
	UDataTable* AIDataTable = AISubsystem->GetPawnDataByTeam(OwnerTeam);
	FAIPawnData* AIData = AIDataTable->FindRow<FAIPawnData>(Key, TEXT("AIPawnDataTable"));
	if(GetOwnerController()->GetResourcesActorComponent()->EnoughResources(AIData->ResourcesNeedToBuy))
	{
		for(const auto& ByArray : AIData->ResourcesNeedToBuy)
		{
			GetOwnerController()->DecreaseResourcesByType(ByArray.Key, ByArray.Value);
		}
		StartSpawnPawn(Key);
	}
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

		if(GetWorld()->GetTimerManager().IsTimerActive(ProgressTimerHandle)) return;
		if(UKismetMathLibrary::IsPointInBox(SpawnLocation, BoxCollision->GetComponentLocation(), Ext))
		{
			StartSpawnPawn(Key);
			return;
		}
		
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &ABaseBuildingActor::OnSpawnPawn, QueueData, SpawnLocation);
		GetWorld()->GetTimerManager().SetTimer(ProgressTimerHandle, TimerDelegate, PawnDataTable->TimeBeforeSpawn, false);
	}
}

void ABaseBuildingActor::OnSpawnPawn(FQueueData QueueData, FVector SpawnLocation)
{
	GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
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

	if(QueueOfSpawn.Num() <= 0)
	{
		if(ImprovementQueue.Num() > 0)
		{
			for(const auto& ByArray : ImprovementQueue)
			{
				StartImprovement(ByArray);
				break;
			}
		}
		return;
	}
	
	GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
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
		GetWorld()->GetTimerManager().SetTimer(ProgressTimerHandle, TimerDelegate, TempData.Value.TimeBeforeSpawn, false);
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

	GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
	QueueOfSpawn.RemoveAllSwap([&](FQueueData Item) -> bool { return Item.QueueId == Key; });
}

void ABaseBuildingActor::GenerateQueueSlots()
{
	auto const HUD = OwnerPlayerController->GetHUD<AStrategyGameBaseHUD>();
	UBaseMatchWidget* MainWidget = HUD->GetMainWidget();
	MainWidget->ClearQueueBorder();
	int32 Index = -1;
	for(const auto& ByArray : QueueOfSpawn)
	{
		if(ByArray.QueueId.IsNone()) continue;
		
		Index++;
		float TempMaxTime = ByArray.Value.TimeBeforeSpawn;
		if(Index == 0 && GetWorld()->GetTimerManager().IsTimerActive(ProgressTimerHandle))
		{
			TempMaxTime = GetWorld()->GetTimerManager().GetTimerRemaining(ProgressTimerHandle);
		}
		
		auto SlotWidget = CreateWidget<UBuildingSpawnProgressSlotBase>(OwnerPlayerController, HUD->GetBuildingProgressSlotClass());
		if(SlotWidget)
		{
			SlotWidget->SetSpawnTime(TempMaxTime);
			SlotWidget->SetId(ByArray.QueueId);
			SlotWidget->SetBuildOwner(this);
			SlotWidget->SetIcon(USyncLoadLibrary::SyncLoadTexture(this, ByArray.Value.Icon));
			MainWidget->AttachSlotToQueue(SlotWidget);
		}
	}

	Index = -1;
	for(const auto& ByArray : ImprovementQueue)
	{
		Index++;
		int32 ImprovementIndex = -1;
		ImprovementIndex = ImprovementLevelInfo.FindByPredicate([&](FImprovementLevelInfo Item) -> bool { return Item.RowName == ByArray.Key; })->CurrentLevel;

		if(!ByArray.Value.ImprovementDataByLevel.IsValidIndex(ImprovementIndex)) continue;
		
		float TempTime = ByArray.Value.ImprovementDataByLevel[ImprovementIndex].ImprovementTime;
		
		if(QueueOfSpawn.Num() > 0)
		{
			if(Index == 0 && GetWorld()->GetTimerManager().IsTimerActive(ProgressTimerHandle))
			{
				TempTime = GetWorld()->GetTimerManager().GetTimerRemaining(ProgressTimerHandle);
			}
		}
		auto SlotWidget = CreateWidget<USpawnProgressSlotBase>(OwnerPlayerController, HUD->GetBuildingProgressSlotClass());
		if(SlotWidget)
		{
			SlotWidget->SetSpawnTime(TempTime);
			SlotWidget->SetId(ByArray.Key);
			SlotWidget->SetIcon(USyncLoadLibrary::SyncLoadTexture(this, ByArray.Value.ImprovementDataByLevel[ImprovementIndex].Icon));
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

	TArray<UActionBaseSlot*> ImprovementSlots;
	if(ImprovementLevelInfo.Num() > 0)
	{
		for(const auto& ByArray : ImprovementLevelInfo)
		{
			if(ImprovementQueue.FindByPredicate([&](FImprovementQueue Item) -> bool { return Item.Key == ByArray.RowName; })) continue;
			auto const ImprovementDataTable = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>()->GetImprovementDataByTeam(OwnerTeam);
			auto const Row = ImprovementDataTable->FindRow<FImprovementData>(ByArray.RowName, *ImprovementDataTable->GetFullName());
			if(!Row) continue;
			if(ByArray.CurrentLevel >= Row->MaxLevelImprovement) continue;

			auto TempImprovementSlot = USyncLoadLibrary::SyncLoadWidget<UImprovementSlotBase>(this, PlayerHUD->GetImprovementSlotClass(), OwnerPlayerController);
			if(TempImprovementSlot)
			{
				TempImprovementSlot->SetIcon(Row->ImprovementDataByLevel[ByArray.CurrentLevel].Icon);
				TempImprovementSlot->SetRowName(ByArray.RowName);
				TempImprovementSlot->SetObjectImprovement(this);
				ImprovementSlots.Add(TempImprovementSlot);
			}
		}
	}
	
	PlayerHUD->CreateActionGrid(SpawnSlots);
	PlayerHUD->AddImprovementWidgetToGrid(ImprovementSlots);
	GenerateQueueSlots();
}

void ABaseBuildingActor::GenerateQueueSlot(const FName& Id)
{
	auto const HUD = OwnerPlayerController->GetHUD<AStrategyGameBaseHUD>();
	UBaseMatchWidget* MainWidget = HUD->GetMainWidget();
	
	UDataTable* TempSpawnData = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>()->GetPawnDataByTeam(OwnerTeam);
	if(!TempSpawnData)
	{ 
		/** @todo add logic for bug fixed */
		return;
	}
	FString const ContextString = TempSpawnData->GetName();
	FAIPawnData* SpawnData = TempSpawnData->FindRow<FAIPawnData>(Id, ContextString);
	
	auto SlotWidget = CreateWidget<UBuildingSpawnProgressSlotBase>(OwnerPlayerController, HUD->GetBuildingProgressSlotClass());
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

void ABaseBuildingActor::Server_RemoveItemFromQueue_Implementation(const FName& QueueId)
{
	int32 const RemoveIndex = QueueOfSpawn.IndexOfByPredicate([&](FQueueData Item) -> bool { return Item.QueueId == QueueId; });
	if(!QueueOfSpawn.IsValidIndex(RemoveIndex)) return;
	
	FQueueData* Finder = QueueOfSpawn.FindByPredicate([&](FQueueData Item) -> bool { return Item.QueueId == QueueId; });
	if(!Finder) return;
	for(const auto& ByArray : Finder->Value.ResourcesNeedToBuy)
	{
		GetOwnerController()->AddResourcesByType(ByArray.Key, ByArray.Value);
	}

	/** if objcet front in queue */
	if(!QueueOfSpawn.IsValidIndex(RemoveIndex - 1))
	{
		QueueOfSpawn.RemoveAt(RemoveIndex);
		RefreshQueue();
		return;
	}
	QueueOfSpawn.RemoveAt(RemoveIndex);
}

void ABaseBuildingActor::Server_Highlighted_Implementation()
{
	bIsHighlighted = true;
}

void ABaseBuildingActor::Server_UnHighlighted_Implementation()
{
	UnHighlighted();
}

bool ABaseBuildingActor::ObjectImprovement_Implementation(const FName& RowName)
{
	auto const SubSystem = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>();
	if(SubSystem && (QueueOfSpawn.Num() + ImprovementQueue.Num()) < 9)
	{
		auto const RowData = *SubSystem->GetImprovementDataByTeam(OwnerTeam)->FindRow<FImprovementData>(RowName, TEXT("DT_ImprovementData"));

		int32 const Index = ImprovementLevelInfo.FindByPredicate([&](FImprovementLevelInfo Item) -> bool
            {  return Item.RowName == RowName; })->CurrentLevel;
		
		auto const ImprovementData = RowData.ImprovementDataByLevel[Index];
		auto const NeedToBuy = RowData.ImprovementDataByLevel[Index].ResourcesNeedToBuy;

		if(OwnerPlayerController->GetResourcesActorComponent()->EnoughResources(NeedToBuy))
		{
			//todo add create improvement slot progress
			Server_BuildImprovement(RowName);
			return true;
		}
	}
	return false;
}

void ABaseBuildingActor::Server_BuildImprovement_Implementation(const FName& RowName)
{
	auto const SubSystem = GetGameInstance()->GetSubsystem<UGameAIPawnSubsystem>();
	if(SubSystem)
	{
		int32 const Index = ImprovementLevelInfo.FindByPredicate([&](FImprovementLevelInfo Item) -> bool {  return Item.RowName == RowName; })->CurrentLevel;

		auto const RowData = *SubSystem->GetImprovementDataByTeam(OwnerTeam)->FindRow<FImprovementData>(RowName, TEXT("DT_ImprovementData"));
		auto const ImprovementData = RowData.ImprovementDataByLevel[Index];
		auto const NeedToBuy = RowData.ImprovementDataByLevel[Index].ResourcesNeedToBuy;
		
		if(OwnerPlayerController->GetResourcesActorComponent()->EnoughResources(NeedToBuy))
		{
			for(const auto& ByArray : NeedToBuy)
			{
				OwnerPlayerController->DecreaseResourcesByType(ByArray.Key, ByArray.Value);
			}
			UAsyncLoadLibrary::AsyncLoadObjectInMemory(this, RowData.ImprovementClass);

			if(QueueOfSpawn.Num() > 0)
			{
				ImprovementQueue.Add(FImprovementQueue(RowName, RowData));
			}
			else
			{
				StartImprovement(FImprovementQueue(RowName, RowData));
			}
		}
	}
}

void ABaseBuildingActor::OnBuildingImprovementCompleted(FImprovementQueue ImprovementData)
{
	GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
	auto const ImprovementObject = NewObject<UPlayerImprovementCharacteristics>(this, ImprovementData.Value.ImprovementClass.Get());
	ImprovementObject->SetOwner(OwnerPlayerController);
	ImprovementObject->ImprovementObjectComplete(this);
	for(auto& ByArray : ImprovementLevelInfo)
	{
		if(ByArray.RowName == ImprovementData.Key)
		{
			ByArray.CurrentLevel++;
			break;
		}
	}
	ImprovementQueue.RemoveAll([&](FImprovementQueue Item) -> bool { return Item.Key == ImprovementData.Key; });
	RefreshQueue();
}

void ABaseBuildingActor::StartImprovement(const FImprovementQueue& ImprovementData)
{
	if(GetLocalRole() != ROLE_Authority) return;

	GetWorld()->GetTimerManager().ClearTimer(ProgressTimerHandle);
	int32 const CurrentLevel = ImprovementLevelInfo.FindByPredicate([&](FImprovementLevelInfo Item) -> bool { return Item.RowName == ImprovementData.Key; })->CurrentLevel;
	float const Time = ImprovementData.Value.ImprovementDataByLevel[CurrentLevel].ImprovementTime;
	FTimerDelegate TimerDel;
	TimerDel.BindUObject(this, &ABaseBuildingActor::OnBuildingImprovementCompleted, ImprovementData);
	GetWorld()->GetTimerManager().SetTimer(ProgressTimerHandle, TimerDel, Time, false);
}