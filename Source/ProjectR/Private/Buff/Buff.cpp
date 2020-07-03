// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Buff.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "TimerManager.h"
#include "Character/ProjectRCharacter.h"

void UBuff::Initialize()
{
	Target = GetTypedOuter<AProjectRCharacter>();
	RecieveInitialize();
}

void UBuff::ApplyBuff()
{
	BeginBuff();
	RecieveBeginBuff();
}

void UBuff::ReleaseBuff()
{
	EndBuff();
	RecieveEndBuff();
}

void UBuff::ApplyBuffWithDuration(float Duration)
{
	ApplyBuff();

	FTimerHandle Handle;
	Target->GetWorld()->GetTimerManager().SetTimer(Handle, [this]
		{
			ReleaseBuff();
		}, Duration, false);
}

void UBuff::Tick(float DeltaSeconds)
{
	RecieveTick(DeltaSeconds);
}
