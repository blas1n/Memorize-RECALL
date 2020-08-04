// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Root.h"

void URoot::OnApply()
{
	bIsRoot = true;
}

void URoot::OnRelease()
{
	bIsRoot = false;
}

bool URoot::IsActivate_Implementation() const
{
	return bIsRoot;
}