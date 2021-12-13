// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BuilgindActors/BaseBuildingActor.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Singleton/SingletonClass.h"
#include "AsyncLoadLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FSpawnBuildingComplete, bool, bResult, const FString&, Reference, AActor*, SpawnActor);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FAsyncLoadActorInMemoryComplete, bool, bResult, const FString&, Reference, TSubclassOf<AActor>, Actor);

class AsyncLoadInMemoryTask;

UCLASS()
class STRATEGYGAME_API UAsyncLoadLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Async  spawn AActor classes */
	template<class T>
	UFUNCTION(BlueprintCallable, Category = "SelectObjectAsset|AsyncSpawn", meta = (WorldContext = "WorldContextObject", DisplayName = "Spawn building (Async)"))
    static bool AsyncSpawnBaseBuilding(UObject* WorldContext, TAssetSubclassOf<T> Class, FTransform Transform, FSpawnBuildingComplete Callback)
	{
		if(Class.IsNull)
		{
			const FString InstigatorName(WorldContext ? WorldContext->GetFullName() : "Unknown");
			UE_LOG(LogTemp, Error, TEXT("Asset is nill --%d"), *InstigatorName);
			return false;
		}
		
		FStreamableManager& AssetLoad = USingletonClass::Get().AssetLoader;
    	FSoftObjectPath const SoftObjectPath = Class.ToSoftObjectPath();
    	AssetLoad.RequestAsyncLoad(SoftObjectPath, FStreamableDelegate::CreateStatic(&UAsyncLoadLibrary::OnAsyncSpawnBaseActorComplete<T>, WorldContext, SoftObjectPath, Transform, Callback));
		return true;
	}

	template<class T>
	static void OnAsyncSpawnBaseActorComplete(UObject* WorldContextObject, FSoftObjectPath SoftObjectPath, FTransform SpawnTransform, FSpawnBuildingComplete Callback)
	{
		T* SpawnActor = nullptr;
	
		/** load object class' */
		UClass* ActorClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *SoftObjectPath.GetAssetName()));
		if(ActorClass)
		{
			SpawnActor = WorldContextObject->GetWorld()->SpawnActor<T>(ActorClass, SpawnTransform);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("UAsyncLoadLibrary::OnAsyncSpawnBaseActorComplete --Spawn actor complite with fail"));
		}
		Callback.Execute(SpawnActor != nullptr, SoftObjectPath.ToString(), SpawnActor);
	}

	template<class T>
	static void AsyncLoadObjectInMemory(UObject* WorldContext, TAssetSubclassOf<T> ClassToLoad)
	{
		if(ClassToLoad.IsNull())
		{
			const FString InstigatorName(WorldContext ? WorldContext->GetFullName() : "Unknown");
			UE_LOG(LogTemp, Error, TEXT("AsyncLoadObjectInMemory --Class to load is null	%d"), *InstigatorName);
			return;
		}

//		if(ClassToLoad.IsValid()) return;

		FStreamableManager& AssetLoader = USingletonClass::Get().AssetLoader;
		FSoftObjectPath const ObjectPath = ClassToLoad.ToSoftObjectPath();
		AssetLoader.RequestAsyncLoad(ObjectPath, FStreamableDelegate::CreateLambda([]() -> void
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("Loaded"));
		}));
	}
};