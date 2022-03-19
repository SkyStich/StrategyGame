// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/ActionSlots/Base/ImprovementSlotBase.h"

#include "Interfaces/ImprovementInterface.h"

FReply UImprovementSlotBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	return FReply::Handled();
}

void UImprovementSlotBase::OnSlotClicked()
{
	if(!ObjectForImprovement || ObjectForImprovement->IsPendingKill()) return;
	if(!ObjectForImprovement->GetClass()->ImplementsInterface(UImprovementInterface::StaticClass())) return;

	if(IImprovementInterface::Execute_ObjectImprovement(ObjectForImprovement, ImprovementRowName))
	{
		RemoveFromParent();
	}
}
