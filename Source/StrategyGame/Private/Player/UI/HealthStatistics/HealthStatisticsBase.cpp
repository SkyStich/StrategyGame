// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/HealthStatistics/HealthStatisticsBase.h"

void UHealthStatisticsBase::SetTarget(AActor* Target)
{
	if(!Target) return;

	HealthComponent = Target->FindComponentByClass<UObjectHealthComponent>();
	if(!HealthComponent) return;

	Init();
}

void UHealthStatisticsBase::ClearTarget()
{
	if(HealthComponent && !HealthComponent->IsPendingKill()) ClearTargetActor();
}
