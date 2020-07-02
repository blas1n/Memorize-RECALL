// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Buff.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"
#include "Character/ProjectRCharacter.h"

void UBuff::Initialize()
{
	Target = GetTypedOuter<AProjectRCharacter>();
}

void UBuff::ApplyBuff(float Duration)
{
	BeginBuff();
	RecieveBeginBuff();

	FTimerHandle Handle;
	Target->GetWorld()->GetTimerManager().SetTimer(Handle, [this]
		{
			EndBuff();
			RecieveEndBuff();
		}, Duration, false);
}
