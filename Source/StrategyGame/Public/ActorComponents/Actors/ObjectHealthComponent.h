// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/HealthInterface.h"
#include "ObjectHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthEnded, AActor*, Actor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthChanged, int32, NewHealth);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class STRATEGYGAME_API UObjectHealthComponent : public UActorComponent, public IHealthInterface
{
	GENERATED_BODY()

	UFUNCTION()
	void OnRep_CurrentHealth();

	UFUNCTION()
	void OnRep_IsDeath();
	
	/**
	 * Set current health
	 *
	 * @param Value Value by which the value will be reduce
	 */
	void DecreaseCurrentHealth(int32 const Value);

	/**
	 * Increase current health
	 * 
	 * @param Value Value bu which the value will be increase
	 */
	void IncreaseCurrentHeath(int32 const Value);

	void StartHealthRegeneration();
	void StopHealthRegeneration();
	void Regeneration();

public:	
	// Sets default values for this component's properties
	UObjectHealthComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsObjectAlive_Implementation() override { return IsAlive(); }

	/**
	 * Set Max health value by default. Call on created object
	 * 
	 * @param Value Value for set new value
	 */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Health|Defauly")
	void SetMaxHealthByDefault(int32 const Value);

	UFUNCTION(BlueprintPure, Category = "HealthComponent")
	int32 GetCurrentObjectHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "HealthComponent")
	int32 GetMaxObjectHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "HealthComponent")
	bool IsAlive() const { return !bDeath; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/**
	 * Call on owner take any damage
	 *
	 * @param DamagedActor Actor which take damage of the owner
	 * @param Damage Base damage
	 * @param DamageType Type damage who have been damage
	 * @param InstigatedBy Controller which be responsible for damage
	 * @param DamageCauser The actor who directly damaged
	 */
	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:

	UPROPERTY(BlueprintAssignable, Category = "HealthComponent")
	FHealthEnded OnHealthEnded;

	UPROPERTY(BlueprintAssignable, Category = "HealthComponent")
	FHealthChanged OnHealthChanged;

private:

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
	int32 CurrentHealth;

	UPROPERTY(Replicated)
	int32 MaxHealth;

	UPROPERTY(ReplicatedUsing = OnRep_IsDeath)
	bool bDeath;
	
	UPROPERTY(Replicated)
	float RegenerationValuePerSec;

	/** time between recovery of health */
	UPROPERTY()
	float RegenerationTime;

	UPROPERTY()
	FTimerHandle RegenerationHandle;
};
