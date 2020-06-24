// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectRCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Parryable.h"
#include "ProjectRPlayerState.h"
#include "WeaponComponent.h"

AProjectRCharacter::AProjectRCharacter()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Hitable"));

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));

	Parrying = nullptr;
	LockedTarget = nullptr;
	bIsCasting = false;
	bCanMoving = true;
	bIsRunning = false;
	bIsDeath = false;
}

void AProjectRCharacter::Attack(AProjectRCharacter* Target, uint8 Damage, AActor* Causer)
{
	if (this == Target) return;

	auto TakingDamage = static_cast<uint8>(Target->
		TakeDamage(Damage, FDamageEvent{}, GetController(), Causer));

	if (TakingDamage > 0u)
		OnAttack.Broadcast(Target, TakingDamage);
}

void AProjectRCharacter::BeginParrying(UObject* InParrying)
{
	if (InParrying->GetClass()->ImplementsInterface(UParryable::StaticClass()))
		Parrying = InParrying;
}

void AProjectRCharacter::EndParrying(UObject* InParrying)
{
	if (Parrying == InParrying)
		Parrying = nullptr;
}

void AProjectRCharacter::SetLockTarget(AProjectRCharacter* Target)
{
	if (LockedTarget == Target) return;

	if (IsValid(LockedTarget))
		LockedTarget->OnLockedOff(this);

	if (IsValid(Target))
		Target->OnLockedOn(this);

	LockedTarget = Target;
	if (LockedTarget) Walk();
}

void AProjectRCharacter::Run()
{
	auto* Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = GetPlayerState<AProjectRPlayerState>()->GetRunSpeed();
	Movement->bOrientRotationToMovement = true;
	SetLockTarget(nullptr);
	bIsRunning = true;
	UnCrouch();
}

void AProjectRCharacter::Walk()
{
	auto* Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = GetPlayerState<AProjectRPlayerState>()->GetWalkSpeed();
	Movement->bOrientRotationToMovement = false;
	bIsRunning = false;
}

void AProjectRCharacter::BeginPlay()
{
	Super::BeginPlay();

	OnAttack.AddDynamic(this, &AProjectRCharacter::HealHealthAndEnergy);

	GetPlayerState<AProjectRPlayerState>()->InitFromDataTable(StatDataRowName);
	Walk();

	TArray<FName> WeaponNames = GetWeaponNames();
	uint8 WeaponNum = FMath::Min(WeaponNames.Num(), 3);

	for (uint8 Idx = 0u; Idx < WeaponNum; ++Idx)
		WeaponComponent->SetNewWeapon(WeaponNames[Idx], Idx);
}

void AProjectRCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	SetControlRotationIfLocked(DeltaSeconds);

	if (!LockedTarget && GetCharacterMovement()->Velocity.SizeSquared() <= 0.0f) return;

	FRotator NewRotation = GetControlRotation();
	NewRotation.Roll = NewRotation.Pitch = 0.0f;
	SetActorRotation(FMath::Lerp(GetActorRotation(), NewRotation, DeltaSeconds * 10.0f));
}

float AProjectRCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	auto Damage = static_cast<uint8>(Super::TakeDamage
		(DamageAmount, DamageEvent, EventInstigator, DamageCauser));

	if (Parrying && IParryable::Execute_IsParryable(Parrying, Damage, EventInstigator, DamageCauser))
	{
		IParryable::Execute_Parry(Parrying, Damage, EventInstigator, DamageCauser);
		return 0.0f;
	}

	auto* MyPlayerState = GetPlayerState<AProjectRPlayerState>();
	MyPlayerState->HealHealth(-Damage);

	if (MyPlayerState->GetHealth() == 0u) Death();
	else OnDamaged.Broadcast(EventInstigator, Damage);

	return Damage;
}

void AProjectRCharacter::HealHealthAndEnergy(AProjectRCharacter* Target, uint8 Damage)
{
	auto* MyPlayerState = GetPlayerState<AProjectRPlayerState>();
	MyPlayerState->HealHealthByDamage(Damage);
	MyPlayerState->HealEnergyByDamage(Damage);
}

void AProjectRCharacter::SetControlRotationIfLocked(float DeltaSeconds)
{
	if (!LockedTarget) return;

	if (LockedTarget->IsDeath())
	{
		SetLockTarget(nullptr);
		return;
	}

	const FVector TargetLocation = LockedTarget->GetActorLocation();
	const FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(GetViewLocation(), TargetLocation);
	const FRotator NowRotation = FMath::Lerp(GetControlRotation(), LookRotation, DeltaSeconds * 5.0f);
	GetController()->SetControlRotation(NowRotation);
}

void AProjectRCharacter::Death()
{
	bIsDeath = true;
	OnDeath.Broadcast(LastHitBy);

	// Enable Ragdoll.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);
}
