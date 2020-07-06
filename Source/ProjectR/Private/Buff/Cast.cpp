// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Cast.h"

void UCast::BeginBuff()
{
	bIsLocked = true;
}

void UCast::EndBuff()
{
	bIsLocked = false;
}

bool UCast::IsActivate_Implementation() const
{
	return bIsLocked;
}
