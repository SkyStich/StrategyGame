// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Enums/ResourcesData.h"
#include "ResourcesActorComponent.generated.h"

//каждое строение добавляет свое значение ресурсов раз в определенный промежуток в ремени(раз в 60 секунд напрмиер) и создать улучшение
//которое будет уменьшать это время

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResourcesUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAddResourcesType, EResourcesType, Type);

USTRUCT(BlueprintType)
struct FResourcesData
{
	GENERATED_BODY()
	
	FResourcesData() : Key(EResourcesType::Food), Value(0) {}
	FResourcesData(EResourcesType Type, int32 ResourcesValue) : Key(Type), Value(ResourcesValue) {}

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	EResourcesType Key;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	int32 Value;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STRATEGYGAME_API UResourcesActorComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	UFUNCTION(Client, Reliable)
	void Client_AddNewResourcesType(EResourcesType Type);

	UFUNCTION()
	void OnRep_ResourcesValues();

public:	

	UResourcesActorComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * Add new resources category in array
	 *
	 * @param	Type	Type for add to array
	 * @param	Value	Base value to add
	 */
	void AddNewResourcesType(const EResourcesType Type, const int32 Value = 0);

	/**
	 * Increase resources value by resources type
	 *
	 * @param	Type	Type resources for add value
	 * @param	Value	resources value for add to array	
	 */
	void IncreaseResourcesValue(const EResourcesType Type, const int32 Value);

	/**
	* Decrease resources value by resources type
	*
	* @param	Type	Type resources for add value
	* @param	Value	resources value for add to array	
	*/
	void DecreaseResourcesValue(const EResourcesType Type, const int32 Value);

	/**
	 * Get current resources value by resources type
	 *
	 * @param	Type	resources type for add value
	 */
	UFUNCTION(BlueprintPure, Category = "Resources")
	int32 GetResourcesValueByKey(EResourcesType Type);

	UFUNCTION(BlueprintPure, Category = "Resources")
	TArray<FResourcesData> GetResourcesValues() const { return ResourcesValue; }

public:

	UPROPERTY(BlueprintAssignable)
	FResourcesUpdated OnResourcesUpdated;

	UPROPERTY(BlueprintAssignable)
	FAddResourcesType OnAddResourcesType;
	
protected:
	
	virtual void BeginPlay() override;
	
private:

	UPROPERTY(ReplicatedUsing = OnRep_ResourcesValues)
	TArray<FResourcesData> ResourcesValue;
};
