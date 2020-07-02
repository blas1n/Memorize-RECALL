// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"
#include "ProjectRCharacter.h"

void UBuff::Initialize()
{
	Target = GetTypedOuter<AProjectRCharacter>();
}

void UBuff::ApplyBuff(float Duration)
{
	OnBeginBuff();

	FTimerHandle Handle;
	Target->GetWorld()->GetTimerManager().SetTimer(Handle, [this]
		{
			OnEndBuff();
		}, Duration, false);
}
