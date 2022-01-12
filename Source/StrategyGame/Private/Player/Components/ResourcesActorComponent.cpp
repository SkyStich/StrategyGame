// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/ResourcesActorComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/RepLayout.h"
// Sets default values for this component's properties
UResourcesActorComponent::UResourcesActorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UResourcesActorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UResourcesActorComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UResourcesActorComponent, ResourcesValue, COND_OwnerOnly);
}

int32 UResourcesActorComponent::GetResourcesValueByKey(EResourcesType Type)
{
	auto const Resources = ResourcesValue.FindByPredicate([&]( FResourcesData Item ) -> bool { return Type == Item.Key; } );
	return Resources ? Resources->Value : 0;
}

void UResourcesActorComponent::IncreaseResourcesValue(const EResourcesType Type, const int32 Value)
{
	if(GetOwnerRole() != ROLE_Authority) return;
	
	for(auto& ByArray : ResourcesValue)
	{
		if(ByArray.Key == Type)
		{
			ByArray.Value += Value;
			OnRep_ResourcesValues();
			return;
		}
	}

	/** if resoures not be find */
	AddNewResourcesType(Type, Value);
}

void UResourcesActorComponent::DecreaseResourcesValue(const EResourcesType Type, const int32 Value)
{
	if(GetOwnerRole() != ROLE_Authority) return;
	
	for(auto& ByArray : ResourcesValue)
	{
		if(ByArray.Key == Type)
		{
			ByArray.Value += Value;
			OnRep_ResourcesValues();
			return;
		}
	}
}

void UResourcesActorComponent::AddNewResourcesType(const EResourcesType Type, const int32 Value)
{
	if(ResourcesValue.ContainsByPredicate([&](FResourcesData Item) -> bool { return Item.Key == Type; })) return;

	ResourcesValue.Add(FResourcesData(Type, Value));
	OnAddResourcesType.Broadcast(Type);
	
	Client_AddNewResourcesType(Type);
}

void UResourcesActorComponent::Client_AddNewResourcesType_Implementation(EResourcesType Type)
{
	OnAddResourcesType.Broadcast(Type);
}

void UResourcesActorComponent::OnRep_ResourcesValues()
{
	OnResourcesUpdated.Broadcast();
}