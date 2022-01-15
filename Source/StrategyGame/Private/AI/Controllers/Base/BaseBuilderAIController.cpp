// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controllers/Base/BaseBuilderAIController.h"

#include "Interfaces/FindObjectTeamInterface.h"

void ABaseBuilderAIController::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(OtherActor->GetClass()->ImplementsInterface(UFindObjectTeamInterface::StaticClass()))
	{

	}
}
