// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/UI/ActionGrid/ActionGridBase.h"

void UActionGridBase::AttachImprovementSlots(const TArray<UActionBaseSlot*> Slots)
{
	auto const Grid = GetUniformGridPanel();
	if(Grid)
	{
		for(int32 i = 15; i > 0; i--)
		{
			int32 const Column = i % 4;
			int32 const Row = i / 4;
			if(Slots.IsValidIndex(15 - i))
			{
				Grid->AddChildToUniformGrid(Slots[15 - i], Row, Column);
			}
		}
	}
}
