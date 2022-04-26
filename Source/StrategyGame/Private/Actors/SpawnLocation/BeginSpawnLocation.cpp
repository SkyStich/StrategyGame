// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/SpawnLocation/BeginSpawnLocation.h"

// Sets default values
ABeginSpawnLocation::ABeginSpawnLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	bReplicates = true;
	NetUpdateFrequency = 1.f;
}

// Called when the game starts or when spawned
void ABeginSpawnLocation::BeginPlay()
{
	Super::BeginPlay();
	
}