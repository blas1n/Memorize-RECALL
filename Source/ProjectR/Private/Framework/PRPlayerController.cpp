// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PRPlayerController.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"

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
	if (auto* MyPawn = GetPawn())
		MyPawn->AddMovementInput(GetDirectionVector(EAxis::X), Value);
}

void APRPlayerController::MoveRight(float Value)
{
	if (auto* MyPawn = GetPawn())
		MyPawn->AddMovementInput(GetDirectionVector(EAxis::Y), Value);
}

void APRPlayerController::InputYaw(float Value)
{
	AddYawInput(Value);
}

void APRPlayerController::InputPitch(float Value)
{
	AddPitchInput(Value);
}

void APRPlayerController::PressDodge()
{
	auto* MyPawn = GetPawn<APRCharacter>();
	if (!MyPawn) return;

	if (MyPawn->IsLocked())
		MyPawn->GetWeaponComponent()->Parry();
	else
		MyPawn->Jump();
}

void APRPlayerController::ReleaseDodge()
{
	auto* MyPawn = GetPawn<ACharacter>();
	if (MyPawn && MyPawn->GetCharacterMovement()->IsFalling())
		MyPawn->StopJumping();
}

void APRPlayerController::Run()
{
	GetPawn<APRCharacter>()->SetMoveState(EMoveState::Run);
}

void APRPlayerController::Walk()
{
	GetPawn<APRCharacter>()->SetMoveState(EMoveState::Walk);
}

void APRPlayerController::AttackWeak()
{
	if (auto* MyPawn = GetPawn<APRCharacter>())
		return MyPawn->GetWeaponComponent()->Attack(false);
}

void APRPlayerController::AttackStrong()
{
	if (auto* MyPawn = GetPawn<APRCharacter>())
		return MyPawn->GetWeaponComponent()->Attack(true);
}

void APRPlayerController::SwapWeapon(uint8 Index)
{
	if (auto* MyPawn = GetPawn<APRCharacter>())
		return MyPawn->GetWeaponComponent()->SwapWeapon(Index);
}

void APRPlayerController::SwapWeapon(float Value)
{
	if (Value == 0.0f) return;

	APRCharacter* MyPawn = GetPawn<APRCharacter>();
	if (!MyPawn) return;

	auto* WeaponComp = MyPawn->GetWeaponComponent();
	const int32 WeaponNum = WeaponComp->GetWeaponNum();
	if (WeaponNum == 0) return;

	int32 Idx = static_cast<int32>(WeaponComp->GetWeaponIndex());
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
