// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Run.h"

void URun::OnApply()
{
	bIsRun = true;
}

void URun::OnRelease()
{
	bIsRun = false;
}

bool URun::IsActivate_Implementation() const
{
	return bIsRun;
}
