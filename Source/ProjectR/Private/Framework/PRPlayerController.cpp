// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PRPlayerController.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Buff/Lock.h"
#include "Buff/Root.h"
#include "Buff/Run.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"
#include "Library/BuffLibrary.h"

DECLARE_DELEGATE_OneParam(FIndexer, uint8);

void APRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis(TEXT("MoveForward"), this, &APRPlayerController::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &APRPlayerController::MoveRight);

	InputComponent->BindAxis(TEXT("Turn"), this, &APRPlayerController::InputYaw);
	InputComponent->BindAxis(TEXT("LookUp"), this, &APRPlayerController::InputPitch);

	InputComponent->BindAxis(TEXT("Swap"), this, &APRPlayerController::SwapWeapon);

	InputComponent->BindAction(TEXT("Dodge"), IE_Pressed, this, &APRPlayerController::PressDodge);
	InputComponent->BindAction(TEXT("Dodge"), IE_Released, this, &APRPlayerController::ReleaseDodge);

	InputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &APRPlayerController::Run);
	InputComponent->BindAction(TEXT("Run"), IE_Released, this, &APRPlayerController::Walk);

	InputComponent->BindAction(TEXT("WeekAttack"), IE_Pressed, this, &APRPlayerController::AttackWeak);
	InputComponent->BindAction(TEXT("StrongAttack"), IE_Pressed, this, &APRPlayerController::AttackStrong);

	InputComponent->BindAction<FIndexer>(TEXT("Weapon1"), IE_Pressed, this, &APRPlayerController::SwapWeapon, static_cast<uint8>(0));
	InputComponent->BindAction<FIndexer>(TEXT("Weapon2"), IE_Pressed, this, &APRPlayerController::SwapWeapon, static_cast<uint8>(1));
	InputComponent->BindAction<FIndexer>(TEXT("Weapon3"), IE_Pressed, this, &APRPlayerController::SwapWeapon, static_cast<uint8>(2));

	InputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &APRPlayerController::Interact);

	//InputComponent->BindAction(TEXT("Lock"), IE_Pressed, this, &APRPlayerController::LockOn);
	//InputComponent->BindAction(TEXT("Lock"), IE_Released, this, &APRPlayerController::LockOff);
}

void APRPlayerController::MoveForward(float Value)
{
	auto* MyPawn = GetPawn<ACharacter>();
	if (MyPawn && !UBuffLibrary::IsActivate<URoot>(MyPawn))
		MyPawn->AddMovementInput(GetDirectionVector(EAxis::X), Value);
}

void APRPlayerController::MoveRight(float Value)
{
	auto* MyPawn = GetPawn<ACharacter>();
	if (MyPawn && !UBuffLibrary::IsActivate<URoot>(MyPawn))
		MyPawn->AddMovementInput(GetDirectionVector(EAxis::Y), Value);
}

void APRPlayerController::InputYaw(float Value)
{
	auto* Lock = UBuffLibrary::GetBuff<ULock>(GetPawn());
	if (!Lock->IsActivate() || !Lock->GetLockedTarget())
		AddYawInput(Value);
}

void APRPlayerController::InputPitch(float Value)
{
	auto* Lock = UBuffLibrary::GetBuff<ULock>(GetPawn());
	if (!Lock->IsActivate() || !Lock->GetLockedTarget())
		AddPitchInput(Value);
}

void APRPlayerController::PressDodge()
{
	auto* MyPawn = GetPawn<APRCharacter>();

	if (UBuffLibrary::IsActivate<ULock>(MyPawn))
		MyPawn->GetWeaponComponent()->Parry();
	else if (!UBuffLibrary::IsActivate<URoot>(MyPawn))
		MyPawn->Jump();
}

void APRPlayerController::ReleaseDodge()
{
	auto* MyPawn = GetPawn<APRCharacter>();
	if (MyPawn && MyPawn->GetCharacterMovement()->IsFalling())
		MyPawn->StopJumping();
}

void APRPlayerController::Run()
{
	UBuffLibrary::ApplyBuff<URun>(GetPawn());
}

void APRPlayerController::Walk()
{
	UBuffLibrary::ReleaseBuff<URun>(GetPawn());
}

void APRPlayerController::AttackWeak()
{
	if (auto* MyPawn = GetPawn<APRCharacter>())
		GetPawn<APRCharacter>()->GetWeaponComponent()->Attack(false);
}

void APRPlayerController::AttackStrong()
{
	if (auto* MyPawn = GetPawn<APRCharacter>())
		GetPawn<APRCharacter>()->GetWeaponComponent()->Attack(true);
}

void APRPlayerController::SwapWeapon(uint8 Index)
{
	if (auto* MyPawn = GetPawn<APRCharacter>())
		GetPawn<APRCharacter>()->GetWeaponComponent()->SwapWeapon(Index);
}

void APRPlayerController::SwapWeapon(float Value)
{
	auto* MyPawn = GetPawn<APRCharacter>();
	if (!MyPawn || Value == 0.0f) return;

	const auto* WeaponComponent = GetPawn<APRCharacter>()->GetWeaponComponent();
	const int32 WeaponNum = WeaponComponent->GetWeaponNum();

	int32 Idx = static_cast<int32>(WeaponComponent->GetWeaponIndex());
	Idx += FMath::RoundFromZero(Value);
	Idx = ((Idx % WeaponNum) + WeaponNum) % WeaponNum;
	
	SwapWeapon(static_cast<uint8>(Idx));
}

void APRPlayerController::Interact()
{
	OnInteract.Broadcast();
}

FVector APRPlayerController::GetDirectionVector(EAxis::Type Axis) const
{
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	return FRotationMatrix(YawRotation).GetUnitAxis(Axis);
}
