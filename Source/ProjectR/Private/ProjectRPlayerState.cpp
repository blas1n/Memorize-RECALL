// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectRPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectRCharacter.h"
#include "ProjectRGameInstance.h"
#include "ProjectRGameState.h"
#include "StatData.h"

void AProjectRPlayerState::InitFromDataTable(const FName& Name)
{
	const auto* GameInstance = Cast<UProjectRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	const auto* DataTable = GameInstance->GetDataTable(TEXT("StatData"));
	const auto& StatData = *DataTable->FindRow<FStatData>(Name, "", false);

	SetMaxHealth(StatData.MaxHealth);
	SetMaxHealth(StatData.HealthHeal);

	SetMaxEnergy(StatData.MaxEnergy);
	SetEnergyHeal(StatData.EnergyHeal);

	SetRunSpeed(StatData.RunSpeed);
	SetWalkSpeed(StatData.WalkSpeed);
	SetCrouchSpeed(StatData.CrouchSpeed);
}

void AProjectRPlayerState::HealHealth(int32 Value) noexcept
{
	Health = FMath::Clamp(Health + Value, 0, MaxHealth);
}

void AProjectRPlayerState::HealHealthByDamage(int32 Damage) noexcept
{
	if (Damage > 0.0f && HealthHeal != 0.0f)
		HealHealth(static_cast<float>(Damage) * HealthHeal);
}

void AProjectRPlayerState::SetMaxHealth(int32 Value, bool bWithCur) noexcept
{
	MaxHealth += Value;
	if (bWithCur) Health += Value;
}

void AProjectRPlayerState::SetHealthHeal(float Value) noexcept
{
	HealthHeal = Value;
}

void AProjectRPlayerState::HealEnergy(int32 Value) noexcept
{
	Energy = FMath::Clamp(Energy + Value, 0, MaxEnergy);
}

void AProjectRPlayerState::HealEnergyByDamage(int32 Damage) noexcept
{
	if (Damage > 0.0f && EnergyHeal != 0.0f)
		HealEnergy(static_cast<float>(Damage) * EnergyHeal);
}

void AProjectRPlayerState::SetMaxEnergy(int32 Value, bool bWithCur) noexcept
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
