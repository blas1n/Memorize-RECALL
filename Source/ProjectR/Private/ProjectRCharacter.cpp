// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectRCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
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

	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 270.0f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));

	Parrying = nullptr;
	LockedTarget = nullptr;

	bCannotMoving = false;
	bIsCasting = false;
	bIsLocking = false;
	bIsTurning = false;
	bIsRunning = false;
	bIsDeath = false;
}

void AProjectRCharacter::Attack(AProjectRCharacter* Target, int32 Damage)
{
	auto TakingDamage = static_cast<int32>(Target->
		TakeDamage(Damage, FDamageEvent{}, GetController(), this));

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
	if (LockedTarget != nullptr && LockedTarget == Target) return;

	if (IsValid(LockedTarget))
		LockedTarget->OnLockedOff(this);

	if (IsValid(Target))
		Target->OnLockedOn(this);

	LockedTarget = Target;
	bIsLocking = true;
	Walk();
}

void AProjectRCharacter::ClearLockTarget()
{
	if (!bIsLocking) return;

	if (IsValid(LockedTarget))
		LockedTarget->OnLockedOff(this);

	LockedTarget = nullptr;
	bIsLocking = false;
}

void AProjectRCharacter::SetTurnRotate(float Yaw)
{
	if (bIsLocking) return;
	bIsTurning = true;
	TurnedYaw = Yaw;
}

void AProjectRCharacter::Jumping()
{
	if (!bCannotMoving)
		Jump();
}

void AProjectRCharacter::Run()
{
	auto* Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = GetPlayerState<AProjectRPlayerState>()->GetRunSpeed();
	ClearLockTarget();
	bIsRunning = true;
}

void AProjectRCharacter::Walk()
{
	auto* Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = GetPlayerState<AProjectRPlayerState>()->GetWalkSpeed();
	bIsRunning = false;
}

void AProjectRCharacter::BeginPlay()
{
	Super::BeginPlay();

	TArray<FName> WeaponNames = GetWeaponNames();
	uint8 WeaponNum = FMath::Min(WeaponNames.Num(), 3);

	for (uint8 Idx = 0u; Idx < WeaponNum; ++Idx)
		WeaponComponent->SetNewWeapon(WeaponNames[Idx], Idx);

	GetPlayerState<AProjectRPlayerState>()->InitFromDataTable(StatDataRowName);
	OnAttack.AddDynamic(this, &AProjectRCharacter::HealHealthAndEnergy);
	Walk();
}

void AProjectRCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsLocking)	Look(DeltaSeconds);
	else Turn(DeltaSeconds);
}

float AProjectRCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	auto Damage = static_cast<int32>(Super::TakeDamage
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

void AProjectRCharacter::HealHealthAndEnergy(AProjectRCharacter* Target, int32 Damage)
{
	auto* MyPlayerState = GetPlayerState<AProjectRPlayerState>();
	MyPlayerState->HealHealthByDamage(Damage);
	MyPlayerState->HealEnergyByDamage(Damage);
}

void AProjectRCharacter::Look(float DeltaSeconds)
{
	if (!LockedTarget) return;

	FRotator LookRotation = UKismetMathLibrary::
		FindLookAtRotation(GetViewLocation(), LockedTarget->GetActorLocation());

	const FRotator NowRotation = FMath::Lerp(GetControlRotation(), LookRotation, DeltaSeconds * 5.0f);
	GetController()->SetControlRotation(NowRotation);
}

void AProjectRCharacter::Turn(float DeltaSeconds)
{
	if (!bIsTurning) return;

	const FRotator CurRotation = GetActorRotation();
	if (FMath::IsNearlyEqual(CurRotation.Yaw, TurnedYaw, 5.0f))
	{
		bIsTurning = false;
		return;
	}

	const FRotator TurnRotation{ CurRotation.Pitch, TurnedYaw, CurRotation.Roll };
	SetActorRotation(FMath::Lerp(CurRotation, TurnRotation, DeltaSeconds * 10.0f));
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
