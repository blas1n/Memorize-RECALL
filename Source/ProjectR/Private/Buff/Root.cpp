// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Root.h"
#include "Buff/Lock.h"
#include "Character/ProjectRCharacter.h"
#include "BuffLibrary.h"

void URoot::OnApply()
{
	if (++Count == 1)
		UBuffLibrary::BlockBuff<ULock>(GetTarget());
}

void URoot::OnRelease()
{
	if (--Count == 0)
		UBuffLibrary::UnblockBuff<ULock>(GetTarget());
}

bool URoot::IsActivate_Implementation() const
{
	return Count > 0;
}