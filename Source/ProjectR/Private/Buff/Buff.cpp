// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Buff.h"
#include "GameFramework/Controller.h"
#include "Character/ProjectRCharacter.h"

void UBuff::BeginPlay()
{
	Target = GetTypedOuter<AProjectRCharacter>();
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
	bIsActivateWithoutBlock = true;
	if (IsBlocked()) return;

	OnApply();
	RecieveOnApply();

	OnApplied.Broadcast();
}

void UBuff::Release()
{
	bIsActivateWithoutBlock = false;
	if (IsBlocked()) return;

	OnRelease();
	RecieveOnRelease();

	OnReleased.Broadcast();
}

void UBuff::Block()
{
	const bool bWasBlock = bIsBlock;
	bIsBlock = true;

	if (!bWasBlock)
	{
		bIsActivateWithoutBlock = IsActivate();
		if (!bIsActivateWithoutBlock) return;

		OnRelease();
		RecieveOnRelease();
	}
}

void UBuff::Unblock()
{
	bIsBlock = false;
	if (bIsActivateWithoutBlock)
		Apply();
}

bool UBuff::IsBlocked() const
{
	return bIsBlock;
}

UWorld* UBuff::GetWorld() const
{
	return Target ? Target->GetWorld() : nullptr;
}
