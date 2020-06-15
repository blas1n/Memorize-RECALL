// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectRPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void FStat::HealStat(uint8 Value) noexcept
{
	CurStat += Value;
	CurStat = FMath::Clamp(CurStat, static_cast<uint8>(0u), MaxStat);
}

void FStat::HealStatByDamage(uint8 Damage) noexcept
{
	if (Damage > 0.0f && StatHeal != 0.0f)
		HealStat(static_cast<float>(Damage) * StatHeal);
}

void FStat::SetMaxStat(uint8 Value, bool bWithCur) noexcept
{
	MaxStat += Value;

	if (bWithCur)
		CurStat += Value;
}

void FStat::SetStatHeal(float Value) noexcept
{
	StatHeal = Value;
}

void AProjectRPlayerState::SetCrouchSpeed(float Value) noexcept
{
	GetPawn<ACharacter>()->GetCharacterMovement()
		->MaxWalkSpeedCrouched = CrouchSpeed = Value;
}
