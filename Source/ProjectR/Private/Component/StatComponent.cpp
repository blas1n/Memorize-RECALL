// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/StatComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Buff/Lock.h"
#include "Buff/Run.h"
#include "Data/StatData.h"

UStatComponent::UStatComponent()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_StatData.DT_StatData'"));
	StatDataTable = DataTable.Object;
}

void UStatComponent::SetLevel(uint8 NewLevel)
{
	Level = NewLevel;
	InitData();
}

void UStatComponent::Heal(float Value)
{
	if (Value != 0.0f)
		MulticastHeal(Value);
}

void UStatComponent::HealByDamage(float Damage)
{
	if (Damage > 0.0f && HealthHeal != 0.0f)
		Heal(Damage * HealthHeal);
}

void UStatComponent::SetMaxHealth(float Value, bool bWithCur)
{
	if (MaxHealth != Value)
		MulticastSetMaxHealth(Value, bWithCur);
}

void UStatComponent::SetHealValue(float Value)
{
	if (HealthHeal != Value)
		MulticastSetHealValue(Value);
}

void UStatComponent::SetRunSpeed(float Value)
{
	if (RunSpeed != Value)
		MulticastSetRunSpeed(Value);
}

void UStatComponent::SetWalkSpeed(float Value)
{
	if (WalkSpeed != Value)
		MulticastSetWalkSpeed(Value);
}

void UStatComponent::SetLockSpeed(float Value)
{
	if (LockSpeed == Value)
		MulticastSetLockSpeed(Value);
}

UBuff* UStatComponent::GetBuff(TSubclassOf<UBuff> BuffClass) const
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

	if (!Ret && HasBegunPlay())
	{
		Ret = NewObject<UBuff>(GetOwner(), BuffClass);
		Ret->BeginPlay();
		Buffs.Add(Ret);
	}

	return Ret;
}

void UStatComponent::BeginPlay()
{
	Super::BeginPlay();
	InitData();

	GetBuff(ULock::StaticClass())->OnApplied.AddDynamic(this, &UStatComponent::OnLockApplied);
	GetBuff(ULock::StaticClass())->OnReleased.AddDynamic(this, &UStatComponent::OnLockReleased);

	GetBuff(URun::StaticClass())->OnApplied.AddDynamic(this, &UStatComponent::OnRunApplied);
	GetBuff(URun::StaticClass())->OnReleased.AddDynamic(this, &UStatComponent::OnRunReleased);
}

void UStatComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	GetBuff(ULock::StaticClass())->OnApplied.RemoveDynamic(this, &UStatComponent::OnLockApplied);
	GetBuff(ULock::StaticClass())->OnReleased.RemoveDynamic(this, &UStatComponent::OnLockReleased);

	GetBuff(URun::StaticClass())->OnApplied.RemoveDynamic(this, &UStatComponent::OnRunApplied);
	GetBuff(URun::StaticClass())->OnReleased.RemoveDynamic(this, &UStatComponent::OnRunReleased);

	for (UBuff* Buff : Buffs)
		Buff->EndPlay();

	Super::EndPlay(EndPlayReason);
}

void UStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (UBuff* Buff : Buffs)
		if (Buff->IsActivate())
			Buff->Tick(DeltaTime);
}

void UStatComponent::InitData()
{
	const FString Key = FString::FromInt(StatKey) + FString::FromInt(Level);
	const auto* Data = StatDataTable->FindRow<FStatData>(FName{ *Key }, TEXT(""));
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found stat data %s!"), *Key);
		return;
	}

	SetMaxHealth(Data->MaxHealth);
	SetHealValue(Data->HealthHeal);

	SetRunSpeed(Data->RunSpeed);
	SetWalkSpeed(Data->WalkSpeed);
	SetLockSpeed(Data->LockSpeed);

	OnChangedLevel.Broadcast();
}

void UStatComponent::MulticastHeal_Implementation(float Value)
{
	Health = FMath::Clamp(Health + Value, 0.0f, MaxHealth);
}

void UStatComponent::MulticastSetMaxHealth_Implementation(float Value, bool bWithCur)
{
	if (bWithCur)
		Health += Value - MaxHealth;

	MaxHealth = Value;
}

void UStatComponent::MulticastSetHealValue_Implementation(float Value)
{
	HealthHeal = Value;
}

void UStatComponent::MulticastSetRunSpeed_Implementation(float Value)
{
	RunSpeed = Value;

	auto* User = Cast<ACharacter>(GetOwner());
	if (User && bIsRunned)
		User->GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
}

void UStatComponent::MulticastSetWalkSpeed_Implementation(float Value)
{
	WalkSpeed = Value;

	auto* User = Cast<ACharacter>(GetOwner());
	if (User && !bIsRunned && !bIsLocked)
		User->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void UStatComponent::MulticastSetLockSpeed_Implementation(float Value)
{
	LockSpeed = Value;

	auto* User = Cast<ACharacter>(GetOwner());
	if (User && !bIsRunned && bIsLocked)
		User->GetCharacterMovement()->MaxWalkSpeed = LockSpeed;
}

void UStatComponent::OnLockApplied()
{
	bIsLocked = true;
	SetMovement();
}

void UStatComponent::OnLockReleased()
{
	bIsLocked = false;
	SetMovement();
}

void UStatComponent::OnRunApplied()
{
	bIsRunned = true;
	SetMovement();
}

void UStatComponent::OnRunReleased()
{
	bIsRunned = false;
	SetMovement();
}

void UStatComponent::SetMovement()
{
	auto* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return;

	auto* Movement = Character->GetCharacterMovement();

	if (bIsRunned)
	{
		Movement->MaxWalkSpeed = RunSpeed;
		Movement->bOrientRotationToMovement = true;
		Movement->bUseControllerDesiredRotation = false;
	}
	else if (bIsLocked)
	{
		Movement->MaxWalkSpeed = LockSpeed;

		if (Cast<ULock>(GetBuff(ULock::StaticClass()))->GetLockedTarget())
		{
			Movement->bOrientRotationToMovement = false;
			Movement->bUseControllerDesiredRotation = true;
		}
	}
	else
	{
		Movement->MaxWalkSpeed = WalkSpeed;
		Movement->bOrientRotationToMovement = true;
		Movement->bUseControllerDesiredRotation = false;
	}
}
