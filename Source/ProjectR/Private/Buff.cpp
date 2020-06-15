// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "BuffStorage.h"
#include "ProjectRCharacter.h"
#include "ProjectRPlayerState.h"

void UBuff::ApplyBuff(AProjectRCharacter* Target, float Duration)
{
	UWorld* World = Target->GetWorld();

	UBuffStorage* BuffStorage = *Target->GetPlayerState<AProjectRPlayerState>()->GetBuffStorages().Find(GetClass());
	OnBeginBuff(World, Target, BuffStorage);

	FTimerHandle Handle;
	World->GetTimerManager().SetTimer(Handle, [this, Target, BuffStorage, World]
		{
			OnEndBuff(World, Target, BuffStorage);
		}, Duration, false);
}
