// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Singleton/SingletonClass.h"
#include "Blueprint/UserWidget.h"
#include "SyncLoadLibrary.generated.h"

/**
 * 
 */
UCLASS()
class STRATEGYGAME_API USyncLoadLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	template<class T>
	UFUNCTION(BlueprintPure, Category = "SyncLoad", meta=(DisplayName="BP_SyncLoadObject"))
	static T* SyncLoadObject(UObject* WorldContext, TAssetPtr<T> Pointer)
	{
		if(Pointer.IsNull())
		{
			FString const InstigatorName = WorldContext ? WorldContext->GetFullName() : "Unknown";
			UE_LOG(LogTemp, Error, TEXT("Asset Ptr is null --%d"), *InstigatorName);
			return nullptr;
		}
		
		FStreamableManager& AssetLoader = USingletonClass::Get().AssetLoader;
		FSoftObjectPath const ObjectPath = Pointer.ToSoftObjectPath();

		/** load object in memory if he in not valid */
		if(!Pointer.IsValid()) Pointer = AssetLoader.LoadSynchronous<T>(ObjectPath);
		return Pointer.Get();
	}

	template<class T>
	UFUNCTION(BlueprintPure, Category = "SyncLoadWidget", meta=(DisplayName="BP_SyncLoadWidget"))
	static T* SyncLoadWidget(UObject* WorldContext, TAssetSubclassOf<T> Pointer, APlayerController* OwningPlayer)
	{
		T* Widget = nullptr;
		if(Pointer.IsNull())
		{
			FString const InstigatorName = WorldContext ? WorldContext->GetFullName() : "Unknown";
            UE_LOG(LogTemp, Error, TEXT("Asset Ptr is null --%d"), *InstigatorName);
			return Widget;
		}
		
		FStreamableManager& AssetLoader = USingletonClass::Get().AssetLoader;
		FSoftObjectPath const ObjectPath = Pointer.ToSoftObjectPath();
			
		/** load object in memory if he in not valid */
		if(!Pointer.IsValid()) AssetLoader.LoadSynchronous(ObjectPath);
			
		UClass* WidgetClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *ObjectPath.ToString()));

		if(WidgetClass)
		{
			if(OwningPlayer)
			{
				Widget = CreateWidget<T>(OwningPlayer, WidgetClass);
			}
			else
			{
				UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull);
				Widget = CreateWidget<T>(World, WidgetClass);
			}
				
			if(Widget)
			{
				Widget->SetFlags(RF_StrongRefOnFrame);
			}
		}
		return Widget;
	}
};


