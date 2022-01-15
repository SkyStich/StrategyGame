// Fill out your copyright notice in the Description page of Project Settings.


#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"

UTexture2D* USyncLoadLibrary::SyncLoadTexture(UObject* WorldContext, TAssetPtr<UTexture2D> Pointer)
{
	if(Pointer.IsNull())
	{
		FString const InstigatorName = WorldContext ? WorldContext->GetFullName() : "Unknown";
		UE_LOG(LogTemp, Error, TEXT("Asset Ptr is null --%d"), *InstigatorName);
		return nullptr;
	}

	if(Pointer.IsValid())
	{
		return Pointer.Get();
	}

	FStreamableManager& Manager = USingletonClass::Get().AssetLoader;
	FSoftObjectPath const ObjectPath = Pointer.ToSoftObjectPath();
	Pointer = Manager.LoadSynchronous<UTexture2D>(ObjectPath);
	return Pointer.Get();
}
