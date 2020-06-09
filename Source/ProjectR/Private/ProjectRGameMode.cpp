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

	for (UClass* Class : TObjectRange<UClass>{})
	{
		if (!Class->IsChildOf<ABuff>() || Class->HasAnyClassFlags(CLASS_Abstract))
			continue;

		FString Name;
		Class->GetName(Name);

		if (Name.StartsWith("TRASHCLASS_") || Name.StartsWith(TEXT("SKEL_")) || Name.StartsWith(TEXT("REINST")))
			continue;

		Buffs.Add(Class, World->SpawnActor<ABuff>(Class, Param));
	}
}