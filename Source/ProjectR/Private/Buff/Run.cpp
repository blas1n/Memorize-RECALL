// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Run.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/ProjectRCharacter.h"
#include "Character/ProjectRPlayerState.h"

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
