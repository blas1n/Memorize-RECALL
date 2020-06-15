// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectRPlayerState.h"

void AProjectRPlayerState::HealHealth(uint8 Value) noexcept
{
	Health += Value;
	Health = FMath::Clamp(Health, static_cast<uint8>(0u), MaxHealth);
}

void AProjectRPlayerState::SetMaxHealth(uint8 Value, bool bWithHealth) noexcept
{
	MaxHealth += Value;

	if (bWithHealth)
		Health += Value;
}

void AProjectRPlayerState::HealEnergy(uint8 Value) noexcept
{
	Energy += Value;
	Energy = FMath::Clamp(Energy, static_cast<uint8>(0u), MaxEnergy);
}

void AProjectRPlayerState::SetMaxEnergy(uint8 Value, bool bWithHealth) noexcept
{
	MaxEnergy += Value;

	if (bWithHealth)
		Energy += Value;
}
