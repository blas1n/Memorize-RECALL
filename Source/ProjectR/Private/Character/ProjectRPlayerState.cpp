// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRPlayerState.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Buff/Buff.h"
#include "Character/ProjectRCharacter.h"
#include "Framework/ProjectRGameInstance.h"
#include "Data/StatData.h"

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

void AProjectRPlayerState::SetRunSpeed(float Value)
{
	RunSpeed = Value;

	auto* User = GetPawn<AProjectRCharacter>();
	if (IsValid(User) && User->IsRunning())
		User->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AProjectRPlayerState::SetWalkSpeed(float Value)
{
	WalkSpeed = Value;

	auto* User = GetPawn<AProjectRCharacter>();
	if (IsValid(User) && !User->IsRunning())
		User->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

UBuff* AProjectRPlayerState::GetBuff(TSubclassOf<UBuff> BuffClass) const
{
	UBuff* Ret = nullptr;;

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
		Ret->Initialize();
		Buffs.Add(Ret);
	}
	
	return Ret;
}

void AProjectRPlayerState::BeginPlay()
{
	Super::BeginPlay();

	auto* MyPawn = GetPawn<AProjectRCharacter>();

	const auto* GameInstance = Cast<UProjectRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	const auto* DataTable = GameInstance->GetDataTable(TEXT("StatData"));
	const auto& StatData = *DataTable->FindRow<FStatData>(MyPawn->GetName(), "", false);

	SetMaxHealth(StatData.MaxHealth);
	SetHealthHeal(StatData.HealthHeal);

	SetMaxEnergy(StatData.MaxEnergy);
	SetEnergyHeal(StatData.EnergyHeal);

	SetRunSpeed(StatData.RunSpeed);
	SetWalkSpeed(StatData.WalkSpeed);

	MyPawn->Walk();
}

void AProjectRPlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	for (UBuff* Buff : Buffs)
		if (Buff->IsActivate())
			Buff->Tick(DeltaSeconds);
}
