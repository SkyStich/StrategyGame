// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/Actors/ObjectHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UObjectHealthComponent::UObjectHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	MaxHealth = 100;
	CurrentHealth = 1;
	RegenerationValuePerSec = 0.f;
	RegenerationTime = 0.25f;
	bDeath = false;

	SetIsReplicatedByDefault(true);
}

void UObjectHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UObjectHealthComponent, CurrentHealth);
	DOREPLIFETIME(UObjectHealthComponent, bDeath);
	DOREPLIFETIME(UObjectHealthComponent, MaxHealth);
}

// Called when the game starts
void UObjectHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if(GetOwnerRole() == ROLE_Authority)
	{
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UObjectHealthComponent::OnTakeAnyDamage);
	}
}

void UObjectHealthComponent::SetMaxHealth(int32 const Value)
{
	if(GetOwnerRole() != ROLE_Authority) return;

	MaxHealth = Value;
}

void UObjectHealthComponent::SetMaxHealthByDefault(int32 const Value)
{
	if(GetOwnerRole() != ROLE_Authority) return;

	MaxHealth = Value;
	CurrentHealth = Value;
}

void UObjectHealthComponent::CalculateBuildConstructHealth(int32 const Value)
{
	if(GetOwnerRole() != ROLE_Authority) return;

	MaxHealth = Value;
	CurrentHealth = Value * 0.1;
}

void UObjectHealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	DecreaseCurrentHealth(Damage);
}

void UObjectHealthComponent::DecreaseCurrentHealth(int32 const Value)
{
	CurrentHealth -= Value;
	OnRep_CurrentHealth();
	
	if(CurrentHealth <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("DEAD!!!!"));
		bDeath = true;
		GetOwner()->SetCanBeDamaged(false);
		OnRep_IsDeath();
	}
}

void UObjectHealthComponent::IncreaseCurrentHeath(int32 const Value)
{
	CurrentHealth += Value;
	OnRep_CurrentHealth();
}

void UObjectHealthComponent::AddRegenerationValuePerSec(float const Value)
{	
	RegenerationValuePerSec += Value;
	
	if(RegenerationValuePerSec <= 0)
	{
		StopHealthRegeneration();
	}
	else
	{
		if(!GetWorld()->GetTimerManager().IsTimerActive(RegenerationHandle))
		{
			StartHealthRegeneration();
		}
	}
}

void UObjectHealthComponent::StartHealthRegeneration()
{
	if(GetOwnerRole() != ROLE_Authority) return;
	
	GetWorld()->GetTimerManager().SetTimer(RegenerationHandle, this, &UObjectHealthComponent::Regeneration, RegenerationTime, true);
}

void UObjectHealthComponent::Regeneration()
{
	if(RegenerationValuePerSec <= 0) StopHealthRegeneration();
	
	IncreaseCurrentHeath(RegenerationValuePerSec * RegenerationTime);
	if(CurrentHealth >= MaxHealth)
	{
		StopHealthRegeneration();
		RegenerationValuePerSec = 0;
		OnRep_RegenerationValue();
	}
}

void UObjectHealthComponent::OnRep_RegenerationValue()
{
	if(RegenerationValuePerSec <= 0)
	{
		OnStopRegeneration.Broadcast();
	}
}


void UObjectHealthComponent::StopHealthRegeneration()
{
	GetWorld()->GetTimerManager().ClearTimer(RegenerationHandle);
}

void UObjectHealthComponent::OnRep_CurrentHealth()
{
	OnHealthChanged.Broadcast(FMath::Clamp(CurrentHealth, 0, MaxHealth));
}

void UObjectHealthComponent::OnRep_IsDeath()
{
	OnHealthEnded.Broadcast(GetOwner());
}