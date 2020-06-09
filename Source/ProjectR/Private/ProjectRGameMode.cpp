// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ProjectRGameMode.h"
#include "Engine/World.h"
#include "UObject/UObjectIterator.h"
#include "Buff.h"
#include "BuffStorage.h"

ABuff* AProjectRGameMode::GetBuffByClass(TSubclassOf<ABuff> BuffClass)
{
	return *Buffs.Find(BuffClass);
}

void AProjectRGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	FActorSpawnParameters Param;
	Param.Owner = this;
	Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (TObjectIterator<UClass> It; It; ++It)
		if (It->IsChildOf(ABuff::StaticClass()) && !It->HasAnyClassFlags(CLASS_Abstract))
			Buffs.Add(*It, World->SpawnActor<ABuff>(*It, Param));
}