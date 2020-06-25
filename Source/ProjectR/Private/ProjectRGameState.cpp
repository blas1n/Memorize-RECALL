// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectRGameState.h"
#include "UObject/UObjectIterator.h"
#include "Buff.h"
#include "BuffStorage.h"

UBuff* AProjectRGameState::GetBuffByClass(TSubclassOf<UBuff> BuffClass)
{
	return *Buffs.Find(BuffClass);
}

void AProjectRGameState::InitBuffStorages(TMap<TSubclassOf<UBuff>, UBuffStorage*>& BuffStorages)
{
	for (const auto& Buff : Buffs)
		BuffStorages.Add(Buff.Key, NewObject<UBuffStorage>(this, Buff.Value->GetBuffStorageClass()));
}

void AProjectRGameState::BeginPlay()
{
	Super::BeginPlay();

	for (UClass* Class : TObjectRange<UClass>{})
	{
		if (!Class->IsChildOf<UBuff>() || Class->HasAnyClassFlags(CLASS_Abstract))
			continue;

		FString Name;
		Class->GetName(Name);

		if (Name.StartsWith("TRASHCLASS_") || Name.StartsWith(TEXT("SKEL_")) || Name.StartsWith(TEXT("REINST")))
			continue;

		Buffs.Add(Class, NewObject<UBuff>(this, Class));
	}
}