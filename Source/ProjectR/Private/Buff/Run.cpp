// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Run.h"
#include "Character/ProjectRCharacter.h"

void URun::BeginBuff()
{
	if (++Count == 1)
		GetTarget()->Walk();
}

void URun::EndBuff()
{
	--Count;
}

bool URun::IsActivate_Implementation() const
{
	return Count > 0;
}
