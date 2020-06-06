// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ProjectRCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Buff.h"
#include "BuffStorage.h"
#include "Parryable.h"
#include "Weapon.h"
#include "WeaponData.h"

AProjectRCharacter::AProjectRCharacter()
	: Super()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Hitable"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	LeftWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftWeapon"));
	LeftWeapon->SetupAttachment(GetMesh(), TEXT("weapon_l"));
	LeftWeapon->SetGenerateOverlapEvents(false);
	LeftWeapon->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	LeftWeapon->SetCollisionProfileName(TEXT("Weapon"));

	RightWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightWeapon"));
	RightWeapon->SetupAttachment(GetMesh(), TEXT("weapon_r"));
	RightWeapon->SetGenerateOverlapEvents(false);
	RightWeapon->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	RightWeapon->SetCollisionProfileName(TEXT("Weapon"));

	Weapon = nullptr;
	WeaponsData = nullptr;
	Health = 0;
	MaxHealth = 0;
	HealthHeal = 0.0f;
	WalkSpeed = 0.0f;
	RunSpeed = 0.0f;
	Parrying = nullptr;
	bIsCasting = false;
}

void AProjectRCharacter::BeginParrying(UObject* InParrying)
{
	if (InParrying->GetClass()->ImplementsInterface(UParryable::StaticClass()))
		Parrying = InParrying;
}

void AProjectRCharacter::EndParrying()
{
	Parrying = nullptr;
}

void AProjectRCharacter::ApplyStun()
{
	NativeOnStunApply();
	OnStunApply();
}

void AProjectRCharacter::ReleaseStun()
{
	NativeOnStunRelease();
	OnStunRelease();
}

int32 AProjectRCharacter::HealHealth(int32 Value)
{
	Health = FMath::Clamp(Health + Value, 0, MaxHealth);
	return Health;
}

int32 AProjectRCharacter::SetMaxHealth(int32 NewMaxHealth)
{
	int32 Diff = NewMaxHealth - MaxHealth;
	MaxHealth = NewMaxHealth;
	Health += Diff;
	return Diff;
}

float AProjectRCharacter::GetSpeed() const noexcept
{
	return GetCharacterMovement()->MaxWalkSpeed;
}

void AProjectRCharacter::SetSpeed(float Speed) noexcept
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void AProjectRCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;
	SetSpeed(WalkSpeed);

	for (TObjectIterator<UClass> It; It; ++It)
		if (It->IsChildOf(ABuff::StaticClass()) && !It->HasAnyClassFlags(CLASS_Abstract))
			BuffStorages.Add(*It, It->GetDefaultObject<ABuff>()->CreateStorage());
}

float AProjectRCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Parrying && IParryable::Execute_IsParryable(Parrying, Damage, EventInstigator, DamageCauser))
	{
		IParryable::Execute_Parry(Parrying, Damage, EventInstigator, DamageCauser);
		return 0.0f;
	}

	Health -= static_cast<int32>(Damage);
	if (Health <= 0) Death();
	
	return Damage;
}

AWeapon* AProjectRCharacter::GenerateWeapon(FName Name)
{
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = GetController();
	SpawnParam.Instigator = this;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWeapon* Ret = GetWorld()->SpawnActor<AWeapon>(SpawnParam);
	const FWeaponData* WeaponData = WeaponsData->FindRow<FWeaponData>(Name, "", false);
	Ret->Initialize(WeaponData);
	return Ret;
}

void AProjectRCharacter::SetWeapon(AWeapon* InWeapon)
{
	if (Weapon)
	{
		Weapon->Unequip();
		Weapon->OnBeginSkill.RemoveDynamic(this, &AProjectRCharacter::BeginCast);
		Weapon->OnEndSkill.RemoveDynamic(this, &AProjectRCharacter::EndCast);
	}

	Weapon = InWeapon;
	if (!Weapon) return;

	Weapon->OnBeginSkill.AddDynamic(this, &AProjectRCharacter::BeginCast);
	Weapon->OnEndSkill.AddDynamic(this, &AProjectRCharacter::EndCast);
	
	FOnAsyncLoadEndedSingle Callback;
	Callback.BindDynamic(this, &AProjectRCharacter::Equip);
	Weapon->RegisterOnAsyncLoadEnded(Callback);
}

void AProjectRCharacter::UseSkill(uint8 Index)
{
	if (Weapon) Weapon->UseSkill(Index);
}

void AProjectRCharacter::Death()
{
	OnDeath.Broadcast(LastHitBy);

	// Enable Ragdoll.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetSimulatePhysics(true);

	FDetachmentTransformRules Rules = FDetachmentTransformRules{ EDetachmentRule::KeepWorld, true };

	LeftWeapon->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	LeftWeapon->SetSimulatePhysics(true);
	LeftWeapon->DetachFromComponent(Rules);

	RightWeapon->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RightWeapon->SetSimulatePhysics(true);
	RightWeapon->DetachFromComponent(Rules);
}

void AProjectRCharacter::Equip()
{
	Weapon->Equip();
}

void AProjectRCharacter::BeginCast()
{
	bIsCasting = true;
}

void AProjectRCharacter::EndCast()
{
	bIsCasting = false;
}