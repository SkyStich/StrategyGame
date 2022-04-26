// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/ActionSlots/Base/ToggleTypeOfBehavior.h"

void UToggleTypeOfBehavior::SetOwnerAIController(ABaseAIController* AIController)
{
	OwnerAIController = AIController;
	OwnerAIController->OnOrderTypeChanged.AddDynamic(this, &UToggleTypeOfBehavior::UpdateIcon);
	CurrentType = AIController->GetAggressiveType();
	UpdateIcon();
}

void UToggleTypeOfBehavior::ToggleBehaviorType(EAIAggressiveType Type)
{
	CurrentType = Type;
	GetWorld()->GetTimerManager().ClearTimer(ChangeOrderDelayHandle);
	FTimerDelegate TimerDel;
	TimerDel.BindLambda([&]() -> void
	{
		OwnerAIController->Server_ToggleAggressiveType(Type);
		GetWorld()->GetTimerManager().ClearTimer(ChangeOrderDelayHandle);
	});
	GetWorld()->GetTimerManager().SetTimer(ChangeOrderDelayHandle, TimerDel, 1.f, false);
	UpdateIcon();
}
