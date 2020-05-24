// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ProjectRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Buff.h"
#include "BuffStorage.h"
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

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

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
	Health = 0;
	MaxHealth = 0;
	HealthHeal = 0.0f;
	WalkSpeed = 0.0f;
	RunSpeed = 0.0f;
}

AWeapon* AProjectRCharacter::GenerateWeapon(FName Name)
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("/Game/WeaponTable"));
	static UDataTable* WeaponTable = DataTable.Succeeded() ? DataTable.Object : nullptr;

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = SpawnParam.Instigator = this;
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AWeapon* Ret = GetWorld()->SpawnActor<AWeapon>(SpawnParam);

	const FWeaponData* WeaponData = WeaponTable->FindRow<FWeaponData>(Name, "", false);
	Ret->Initialize(WeaponData);

	return Ret;
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
}

float AProjectRCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= static_cast<int32>(DamageAmount);
	if (Health <= 0) Death();
	
	return Damage;
}

void AProjectRCharacter::PressSkill(uint8 Index)
{
	if (Weapon) Weapon->PressSkill(Index);
}

void AProjectRCharacter::ReleaseSkill(uint8 Index)
{
	if (Weapon) Weapon->ReleaseSkill(Index);
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