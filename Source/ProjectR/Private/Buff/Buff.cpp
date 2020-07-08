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
	bIsActivateBeforeBlock = true;
	if (IsBlocked()) return;

	OnApply();
	RecieveOnApply();
}

void UBuff::Release()
{
	bIsActivateBeforeBlock = false;
	if (IsBlocked()) return;

	OnRelease();
	RecieveOnRelease();
}

void UBuff::Block()
{
	if (++BlockCount > 1)
		return;

	bIsActivateBeforeBlock = IsActivate();
	if (!bIsActivateBeforeBlock) return;

	OnRelease();
	RecieveOnRelease();
}

void UBuff::Unblock()
{
	if (--BlockCount > 0)
		return;

	if (bIsActivateBeforeBlock)
		Apply();
}

bool UBuff::IsBlocked() const
{
	return BlockCount > 0;
}
