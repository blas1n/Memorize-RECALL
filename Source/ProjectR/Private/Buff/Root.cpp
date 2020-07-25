// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Root.h"
#include "Buff/Lock.h"
#include "Buff/Run.h"
#include "Character/ProjectRCharacter.h"
#include "Library/BuffLibrary.h"

void URoot::OnApply()
{
	bIsRoot = true;
	UBuffLibrary::BlockBuff<ULock>(GetTarget());
	UBuffLibrary::BlockBuff<URun>(GetTarget());
}

void URoot::OnRelease()
{
	bIsRoot = false;
	UBuffLibrary::UnblockBuff<ULock>(GetTarget());
	UBuffLibrary::UnblockBuff<URun>(GetTarget());
}

bool URoot::IsActivate_Implementation() const
{
	return bIsRoot;
}