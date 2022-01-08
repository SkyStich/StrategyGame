// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/Building/HighLightedSlotBase.h"
#include "Player/UI/Building/ToolTip/HighlightedToolTipBase.h"
#include "BlueprintFunctionLibraries/SyncLoadLibrary.h"
#include "UObject/ConstructorHelpers.h"

UHighLightedSlotBase::UHighLightedSlotBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<UUserWidget>HighlightedToolTipFinder(TEXT("/Game/Blueprints/UI/InteractionWidgets/ToolTip/W_HighlightedToolTip"));
	if(HighlightedToolTipFinder.Succeeded()) HighlightedToolTip = HighlightedToolTipFinder.Class;
	
}

UUserWidget* UHighLightedSlotBase::OnToolTipWidgetEvent()
{
	UHighlightedToolTipBase* ToolTipSlot = USyncLoadLibrary::SyncLoadWidget<UHighlightedToolTipBase>(this, HighlightedToolTip, GetOwningPlayer());
	ToolTipSlot->BuildingName = GetObjectName();
	ToolTipSlot->Description = GetObjectDescription();
	return ToolTipSlot;
}

UTexture2D* UHighLightedSlotBase::GetIcon()
{
	return USyncLoadLibrary::SyncLoadObject<UTexture2D>(this, GetAssetIconPtr());
}

