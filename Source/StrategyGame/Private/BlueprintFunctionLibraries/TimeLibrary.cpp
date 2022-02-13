// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintFunctionLibraries/TimeLibrary.h"

float UTimeLibrary::GetTimeRate(UObject* WorldContext, const FTimerHandle& Timer)
{
	if(!WorldContext)
	{
		UE_LOG(LogTemp, Error, TEXT("WorkdContext is null --UTimeLibrary::GetTimeRate()"));
		return 0.f;
	}
	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext);
	return World->GetTimerManager().GetTimerRate(Timer);
}
