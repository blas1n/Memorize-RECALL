// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/StatComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Buff/Lock.h"
#include "Buff/Run.h"
#include "Data/StatData.h"

UStatComponent::UStatComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_StatData.DT_StatData'"));
	StatDataTable = DataTable.Object;
}

void UStatComponent::SetLevel(uint8 NewLevel)
{
	check(GetOwnerRole() == ENetRole::ROLE_Authority);
	MulticastSetLevel(NewLevel);
}

void UStatComponent::Heal(float Value)
{
	check(GetOwnerRole() == ENetRole::ROLE_Authority);
	Health = FMath::Clamp(Health + Value, 0.0f, MaxHealth);
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

	if (GetOwnerRole() == ENetRole::ROLE_Authority)
		SetLevel(Level);

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

void UStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UStatComponent, Health);
}

void UStatComponent::MulticastSetLevel_Implementation(uint8 NewLevel)
{
	Level = NewLevel;

	const FString Key = FString::FromInt(StatKey) + FString::FromInt(Level);
	const auto* Data = StatDataTable->FindRow<FStatData>(FName{ *Key }, TEXT(""), false);
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found stat data %s!"), *Key);
		return;
	}

	Health += Data->MaxHealth - MaxHealth;
	MaxHealth = Data->MaxHealth;

	RunSpeed = Data->RunSpeed;
	WalkSpeed = Data->WalkSpeed;
	LockSpeed = Data->LockSpeed;
	SetMovement();

	OnChangedLevel.Broadcast(Level);
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
