// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Buff/Run.h"
#include "Character/ProjectRCharacter.h"
#include "Data/StatData.h"
#include "Framework/ProjectRGameInstance.h"
#include "BuffLibrary.h"

AProjectRPlayerState::AProjectRPlayerState()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AProjectRPlayerState::HealHealth(int32 Value) noexcept
{
	Health = FMath::Clamp(Health + Value, 0, MaxHealth);
}

void AProjectRPlayerState::HealHealthByDamage(int32 Damage) noexcept
{
	if (Damage > 0.0f && HealthHeal != 0.0f)
		HealHealth(FMath::TruncToInt(static_cast<float>(Damage) * HealthHeal));
}

void AProjectRPlayerState::SetMaxHealth(int32 Value, bool bWithCur) noexcept
{
	if (bWithCur) Health += Value - MaxHealth;
	MaxHealth = Value;
	Health = FMath::Min(Health, MaxHealth);
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
		HealEnergy(FMath::TruncToInt(static_cast<float>(Damage) * EnergyHeal));
}

void AProjectRPlayerState::SetMaxEnergy(int32 Value, bool bWithCur) noexcept
{
	if (bWithCur) Energy += Value - MaxEnergy;
	MaxEnergy = Value;
	Energy = FMath::Min(Health, MaxEnergy);
}

void AProjectRPlayerState::SetEnergyHeal(float Value) noexcept
{
	EnergyHeal = Value;
}

void AProjectRPlayerState::SetRunSpeed(float Value)
{
	RunSpeed = Value;

	auto* User = GetPawn<AProjectRCharacter>();
	if (User && UBuffLibrary::IsActivate<URun>(User))
		User->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AProjectRPlayerState::SetWalkSpeed(float Value)
{
	WalkSpeed = Value;

	auto* User = GetPawn<AProjectRCharacter>();
	if (User && !UBuffLibrary::IsActivate<URun>(User))
		User->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AProjectRPlayerState::SetLookSpeed(float Value)
{
	LookSpeed = Value;

	auto* User = GetPawn<AProjectRCharacter>();
	if (User && !UBuffLibrary::IsActivate<URun>(User))
		User->GetCharacterMovement()->MaxWalkSpeed = LookSpeed;
}

UBuff* AProjectRPlayerState::GetBuff(TSubclassOf<UBuff> BuffClass) const
{
	UBuff* Ret = nullptr;

	for (UBuff* Buff : Buffs)
	{
		if (Buff->GetClass()->IsChildOf(BuffClass))
		{
			Ret = Buff;
			break;
		}
	}

	if (!Ret)
	{
		Ret = NewObject<UBuff>(GetPawn(), BuffClass);
		Ret->BeginPlay();
		Buffs.Add(Ret);
	}
	
	return Ret;
}

void AProjectRPlayerState::BeginPlay()
{
	Super::BeginPlay();

	auto* MyPawn = GetPawn<AProjectRCharacter>();
	if (!MyPawn) return;

	const auto* GameInstance = Cast<UProjectRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	const auto* DataTable = GameInstance->GetDataTable(TEXT("StatData"));
	const auto& StatData = *DataTable->FindRow<FStatData>(MyPawn->GetName(), "", false);

	SetMaxHealth(StatData.MaxHealth);
	SetHealthHeal(StatData.HealthHeal);

	SetMaxEnergy(StatData.MaxEnergy);
	SetEnergyHeal(StatData.EnergyHeal);

	SetRunSpeed(StatData.RunSpeed);
	SetWalkSpeed(StatData.WalkSpeed);
}

void AProjectRPlayerState::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	for (UBuff* Buff : Buffs)
		Buff->EndPlay();

	Super::EndPlay(EndPlayReason);
}

void AProjectRPlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for (UBuff* Buff : Buffs)
		if (Buff->IsActivate())
			Buff->Tick(DeltaSeconds);
}
