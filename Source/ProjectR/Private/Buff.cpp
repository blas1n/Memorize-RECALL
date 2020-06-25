// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "BuffStorage.h"
#include "ProjectRCharacter.h"
#include "ProjectRPlayerState.h"

void UBuff::ApplyBuff(AProjectRCharacter* Target, float Duration)
{
	auto* BuffStorage = *Target->GetPlayerState<AProjectRPlayerState>()->GetBuffStorages().Find(GetClass());
	OnBeginBuff(Target, BuffStorage);

	FTimerHandle Handle;
	Target->GetWorld()->GetTimerManager().SetTimer(Handle, [this, Target, BuffStorage]
		{
			OnEndBuff(Target, BuffStorage);
		}, Duration, false);
}
