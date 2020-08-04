// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Buff.h"
#include "GameFramework/Controller.h"
#include "Framework/PRCharacter.h"

void UBuff::BeginPlay()
{
	Target = GetTypedOuter<APRCharacter>();
	RecieveBeginPlay();
}

void UBuff::EndPlay()
{
	RecieveEndPlay();
}

void UBuff::Tick(float DeltaSeconds)
{
	RecieveTick(DeltaSeconds);
}

void UBuff::Apply()
{
	const bool bIsAlreadActive = IsActivate();

	OnApply();
	RecieveOnApply();

	if (!bIsAlreadActive && IsActivate())
		OnApplied.Broadcast();
}

void UBuff::Release()
{
	const bool bIsAlreadActive = IsActivate();

	OnRelease();
	RecieveOnRelease();

	if (bIsAlreadActive && !IsActivate())
		OnReleased.Broadcast();
}

UWorld* UBuff::GetWorld() const
{
	return Target ? Target->GetWorld() : nullptr;
}
