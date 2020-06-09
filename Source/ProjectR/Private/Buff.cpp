// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff.h"
#include "BuffStorage.h"
#include "ProjectRCharacter.h"
#include "TimerManager.h"

ABuff::ABuff()
	: Super()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABuff::ApplyBuff(AProjectRCharacter* Target, float Duration)
{
	UBuffStorage* BuffStorage = *Target->GetBuffStorages().Find(GetClass());
	OnBeginBuff(Target, BuffStorage);

	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, [this, Target, BuffStorage]
		{
			OnEndBuff(Target, BuffStorage);
		}, Duration, false);
}
