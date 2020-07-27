// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRPlayerState.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Buff/Lock.h"
#include "Buff/Run.h"
#include "Character/ProjectRCharacter.h"
#include "Data/StatData.h"

AProjectRPlayerState::AProjectRPlayerState()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_StatData.DT_StatData'"));
	StatDataTable = DataTable.Object;
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
	if (User && bIsRunned)
		User->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void AProjectRPlayerState::SetWalkSpeed(float Value)
{
	WalkSpeed = Value;

	auto* User = GetPawn<AProjectRCharacter>();
	if (User && !bIsRunned && !bIsLocked)
		User->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AProjectRPlayerState::SetLockSpeed(float Value)
{
	LockSpeed = Value;

	auto* User = GetPawn<AProjectRCharacter>();
	if (User && !bIsRunned && bIsLocked)
		User->GetCharacterMovement()->MaxWalkSpeed = LockSpeed;
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

	const FString Key = FString::FromInt(MyPawn->GetKey()) + FString::FromInt(MyPawn->GetLevel());

	const auto* Data = StatDataTable->FindRow<FStatData>(FName{ *Key }, TEXT(""));
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found stat data %s!"), *Key);
		return;
	}

	SetMaxHealth(Data->MaxHealth);
	SetHealthHeal(Data->HealthHeal);

	SetMaxEnergy(Data->MaxEnergy);
	SetEnergyHeal(Data->EnergyHeal);

	SetRunSpeed(Data->RunSpeed);
	SetWalkSpeed(Data->WalkSpeed);
	SetLockSpeed(Data->LockSpeed);

	GetBuff(ULock::StaticClass())->OnApplied.AddDynamic(this, &AProjectRPlayerState::OnLockApplied);
	GetBuff(ULock::StaticClass())->OnReleased.AddDynamic(this, &AProjectRPlayerState::OnLockReleased);

	GetBuff(URun::StaticClass())->OnApplied.AddDynamic(this, &AProjectRPlayerState::OnRunApplied);
	GetBuff(URun::StaticClass())->OnReleased.AddDynamic(this, &AProjectRPlayerState::OnRunReleased);
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

void AProjectRPlayerState::OnLockApplied()
{
	bIsLocked = true;
	SetMovement();
}

void AProjectRPlayerState::OnLockReleased()
{
	bIsLocked = false;
	SetMovement();
}

void AProjectRPlayerState::OnRunApplied()
{
	bIsRunned = true;
	SetMovement();
}

void AProjectRPlayerState::OnRunReleased()
{
	bIsRunned = false;
	SetMovement();
}

void AProjectRPlayerState::SetMovement()
{
	auto* Movement = GetPawn<ACharacter>()->GetCharacterMovement();
	if (!Movement) return;

	if (bIsRunned)
	{
		Movement->MaxWalkSpeed = RunSpeed;
		Movement->bOrientRotationToMovement = true;
		Movement->bUseControllerDesiredRotation = false;
	}
	else if (bIsLocked)
	{
		Movement->MaxWalkSpeed = LockSpeed;
		Movement->bOrientRotationToMovement = false;
		Movement->bUseControllerDesiredRotation = true;
	}
	else
	{
		Movement->MaxWalkSpeed = WalkSpeed;
		Movement->bOrientRotationToMovement = true;
		Movement->bUseControllerDesiredRotation = false;
	}
}
