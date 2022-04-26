// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Pawns/Base/BaseAIPawn.h"
#include "Player/UI/ActionSlots/Base/ActionSpawnBuildingSlot.h"
#include "DrawDebugHelpers.h"
#include "AI/Controllers/Base/BaseAIController.h"
#include "Player/HUD/StrategyGameBaseHUD.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "ActorComponents/Actors/ObjectHealthComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseAIPawn::ABaseAIPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bReplicates = true;
	NetUpdateFrequency = 10.f;
	AIControllerClass = ABaseAIController::StaticClass();	
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->MovementState.bCanJump = false;
	GetCharacterMovement()->MovementState.bCanCrouch = false;

	ObjectHealthComponent = CreateDefaultSubobject<UObjectHealthComponent>(TEXT("ObjectHealthComponent"));

	bUseControllerRotationYaw = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorld;
}

// Called when the game starts or when spawned
void ABaseAIPawn::BeginPlay()
{
	Super::BeginPlay();
	
	ObjectHealthComponent->OnHealthEnded.AddDynamic(this, &ABaseAIPawn::OnHealthEndedEvent);
}

void ABaseAIPawn::OnHealthEndedEvent(AActor* Actor)
{
	if(GetLocalRole() == ROLE_Authority)
	{
		/** test */
		Controller->StopMovement();
		StopAttack();
		FTimerHandle DestroyHandle;
		FTimerDelegate TimerDel;
		TimerDel.BindLambda([&]() -> void
		{
			Destroy();
		});
		GetWorld()->GetTimerManager().SetTimer(DestroyHandle, TimerDel, 3.f, false);
	}
}

void ABaseAIPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(GetLocalRole() != ROLE_Authority) return;
#if UE_EDITOR
	DrawDebugSphere(GetWorld(), GetActorLocation(), AttackData.BaseDistanceForAttack, 8, FColor::Red, false, 0.f);
#endif
}


void ABaseAIPawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseAIPawn, OwnerTeam);
	DOREPLIFETIME_CONDITION(ABaseAIPawn, PawnName, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ABaseAIPawn, PawnRowName, COND_InitialOnly);
}

bool ABaseAIPawn::GiveOrderToTargetPawn_Implementation(const FVector& LocationToMove, AActor* ActorToMove)
{
	if(!ObjectHealthComponent->IsAlive()) return false;

	StopAttack();
	if(GetLocalRole() == ROLE_Authority)
	{
		auto const AIController = GetController<ABaseAIController>();
		if(AIController)
		{
			AIController->MoveToGiveOrder(LocationToMove, ActorToMove);
			return true;
		}
	}
	return false;
}

void ABaseAIPawn::SetHealthDefault(float const Value)
{
	if(GetLocalRole() != ROLE_Authority) return;

	ObjectHealthComponent->SetMaxHealthByDefault(Value);
}

void ABaseAIPawn::InitPawn(const FAIPawnData& PawnData)
{
	if(GetLocalRole() == ROLE_Authority)
    {
    	PawnRowName = PawnData.Id;
		PawnName = PawnData.ObjectName;

		/** init attack data */
		AttackData.BaseDamage = PawnData.BaseDamage;
		AttackData.BaseDelayBeforeUsed = PawnData.BaseDelayBeforeAttack;
		AttackData.BaseDistanceForAttack = PawnData.BaseRangeAttack;
		SetHealthDefault(PawnData.BaseHealth);
    }
}

void ABaseAIPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

}

void ABaseAIPawn::HighlightedActor_Implementation(AStrategyGameBaseHUD* PlayerHUD)
{
	if(GetLocalRole() == ROLE_Authority) return;
	
	auto const DestroySlot = CreateWidget<UDestroyHighlightedObjectSlot>(PlayerHUD->GetOwningPlayerController(), PlayerHUD->GetDestroyObjectSlot());
	if(DestroySlot)
	{
		DestroySlot->AddToViewport();
		PlayerHUD->AddActionSlotByIndex(DestroySlot, 3, 3);
	}

	auto const ToggleAggressiveTypeSlot = CreateWidget<UToggleTypeOfBehavior>(PlayerHUD->GetOwningPlayerController(), PlayerHUD->GetToggleBehaviorType());
	if(ToggleAggressiveTypeSlot)
	{
		ToggleAggressiveTypeSlot->AddToViewport();
			
		auto const AIController = Cast<ABaseAIController>(Controller);
		ToggleAggressiveTypeSlot->SetOwnerAIController(AIController);
		PlayerHUD->AddActionSlotByIndex(ToggleAggressiveTypeSlot, 2, 2);
	}
}
