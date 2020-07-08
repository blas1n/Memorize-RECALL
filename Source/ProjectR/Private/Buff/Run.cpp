// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Run.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/ProjectRCharacter.h"
#include "Character/ProjectRPlayerState.h"

void URun::OnApply()
{
	bIsRun = true;

	auto* PlayerState = GetTarget()->GetPlayerState<AProjectRPlayerState>();
	if (!PlayerState) return;

	auto* Movement = GetTarget()->GetCharacterMovement();
	Movement->MaxWalkSpeed = PlayerState->GetRunSpeed();
}

void URun::OnRelease()
{
	bIsRun = false;

	auto* PlayerState = GetTarget()->GetPlayerState<AProjectRPlayerState>();
	if (!PlayerState) return;

	auto* Movement = GetTarget()->GetCharacterMovement();
	Movement->MaxWalkSpeed = PlayerState->GetWalkSpeed();
}

bool URun::IsActivate_Implementation() const
{
	return bIsRun;
}
