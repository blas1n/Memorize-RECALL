// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectRPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Buff.h"
#include "ProjectRCharacter.h"
#include "ProjectRGameInstance.h"
#include "StatData.h"

void AProjectRPlayerState::InitFromDataTable(const FName& Name)
{
	const auto* GameInstance = Cast<UProjectRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	const auto* DataTable = GameInstance->GetDataTable(TEXT("StatData"));
	const auto& StatData = *DataTable->FindRow<FStatData>(Name, "", false);

	SetMaxHealth(StatData.MaxHealth);
	SetHealthHeal(StatData.HealthHeal);

	SetMaxEnergy(StatData.MaxEnergy);
	SetEnergyHeal(StatData.EnergyHeal);

	SetRunSpeed(StatData.RunSpeed);
	SetWalkSpeed(StatData.WalkSpeed);
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
	if (bWithCur)
	{
		const int32 Delta = Value - Health;
		Health += Delta;
	}

	MaxHealth = Value;
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
	if (bWithCur)
	{
		const int32 Delta = Value - Energy;
		Energy += Delta;
	}

	MaxEnergy = Value;
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

UBuff* AProjectRPlayerState::GetBuff(TSubclassOf<UBuff> BuffClass)
{
	UBuff** Buff = Buffs.Find(BuffClass);

	if (!Buff)
		Buff = &Buffs.Add(BuffClass, NewObject<UBuff>(GetPawn()));
	
	return *Buff;
}
