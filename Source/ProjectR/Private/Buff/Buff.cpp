// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Buff.h"
#include "Engine/NetDriver.h"
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
	ServerApply();
}

void UBuff::Release()
{
	ServerRelease();
}

UWorld* UBuff::GetWorld() const
{
	return Target ? Target->GetWorld() : nullptr;
}

void UBuff::MulticastApply_Implementation()
{
	const bool bIsAlreadActive = IsActivate();

	OnApply();
	RecieveOnApply();

	if (!bIsAlreadActive && IsActivate())
		OnApplied.Broadcast();
}

void UBuff::MulticastRelease_Implementation()
{
	const bool bIsAlreadActive = IsActivate();

	OnRelease();
	RecieveOnRelease();

	if (bIsAlreadActive && !IsActivate())
		OnReleased.Broadcast();
}
