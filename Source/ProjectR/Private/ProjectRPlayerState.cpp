// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectRPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectRGameState.h"

void AProjectRPlayerState::HealHealth(uint8 Value) noexcept
{
	Health += Value;
	Health = FMath::Clamp(Health, static_cast<uint8>(0u), MaxHealth);
}

void AProjectRPlayerState::HealHealthByDamage(uint8 Damage) noexcept
{
	if (Damage > 0.0f && HealthHeal != 0.0f)
		HealHealth(static_cast<float>(Damage) * HealthHeal);
}

void AProjectRPlayerState::SetMaxHealth(uint8 Value, bool bWithCur) noexcept
{
	MaxHealth += Value;
	if (bWithCur) Health += Value;
}

void AProjectRPlayerState::SetHealthHeal(float Value) noexcept
{
	HealthHeal = Value;
}

void AProjectRPlayerState::HealEnergy(uint8 Value) noexcept
{
	Energy += Value;
	Energy = FMath::Clamp(Energy, static_cast<uint8>(0u), MaxEnergy);
}

void AProjectRPlayerState::HealEnergyByDamage(uint8 Damage) noexcept
{
	if (Damage > 0.0f && EnergyHeal != 0.0f)
		HealEnergy(static_cast<float>(Damage) * EnergyHeal);
}

void AProjectRPlayerState::SetMaxEnergy(uint8 Value, bool bWithCur) noexcept
{
	MaxEnergy += Value;
	if (bWithCur) Energy += Value;
}

void AProjectRPlayerState::SetEnergyHeal(float Value) noexcept
{
	EnergyHeal = Value;
}

void AProjectRPlayerState::SetRunSpeed(float Value) noexcept
{
	RunSpeed = Value;

	auto* User = GetPawn<AProjectRCharacter>();
	if (IsValid(User) && User->IsRunning())
		User->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AProjectRPlayerState::SetWalkSpeed(float Value) noexcept
{
	WalkSpeed = Value;

	auto* User = GetPawn<AProjectRCharacter>();
	if (IsValid(User) && !User->IsRunning())
		User->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AProjectRPlayerState::SetCrouchSpeed(float Value) noexcept
{
	GetPawn<ACharacter>()->GetCharacterMovement()
		->MaxWalkSpeedCrouched = CrouchSpeed = Value;
}

void AProjectRPlayerState::BeginPlay()
{
	auto* GameState = Cast<AProjectRGameState>
		(UGameplayStatics::GetGameState(GetWorld()));

	GameState->InitBuffStorages(BuffStorages);
}
