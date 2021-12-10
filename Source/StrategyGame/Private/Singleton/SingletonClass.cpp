// Fill out your copyright notice in the Description page of Project Settings.


#include "Singleton/SingletonClass.h"

USingletonClass& USingletonClass::Get()
{
	USingletonClass* Singleton = Cast<USingletonClass>(GEngine->GameSingleton);
	return Singleton ? *Singleton : *NewObject<USingletonClass>(USingletonClass::StaticClass());
}