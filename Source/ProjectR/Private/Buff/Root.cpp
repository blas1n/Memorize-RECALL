// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Root.h"

void URoot::BeginBuff()
{
	bIsRooted = true;
}

void URoot::EndBuff()
{
	bIsRooted = false;
}

bool URoot::IsActivate_Implementation() const
{
	return bIsRooted;
}
