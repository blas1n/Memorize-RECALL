// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "Weapon.h"

DECLARE_DELEGATE_OneParam(FIndexer, uint8);
DECLARE_DELEGATE_OneParam(FSpeedSetter, float);

APlayerCharacter::APlayerCharacter()
	: Super()
{
	Weapons.SetNum(3);
	Energy = 0;
	MaxEnergy = 0;
	EnergyHeal = 0;
	CurWeaponIndex = 0;
}

void APlayerCharacter::EquipWeapon(FName Name, uint8 Index)
{
	Weapons[Index] = GenerateWeapon(Name);

	bool bIsFirstWeapon = true;

	for (int I = 0; I < 3; ++I)
		bIsFirstWeapon &= I == Index || Weapons[I] == nullptr;

	if (bIsFirstWeapon)
		SwapWeapon(Index);
}

int32 APlayerCharacter::HealEnergy(int32 Value)
{
	Energy = FMath::Clamp(Energy + Value, 0, MaxEnergy);
	return Energy;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Energy = MaxEnergy;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis(TEXT("Swap"), this, &APlayerCharacter::SwapWeapon);

	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction<FSpeedSetter>(TEXT("Sprint"), IE_Pressed, this, &APlayerCharacter::SetSpeed, GetRunSpeed());
	PlayerInputComponent->BindAction<FSpeedSetter>(TEXT("Sprint"), IE_Released, this, &APlayerCharacter::SetSpeed, GetWalkSpeed());

	PlayerInputComponent->BindAction<FIndexer>(TEXT("Weapon1"), IE_Pressed, this, &APlayerCharacter::SwapWeapon, static_cast<uint8>(0));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Weapon2"), IE_Pressed, this, &APlayerCharacter::SwapWeapon, static_cast<uint8>(1));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Weapon3"), IE_Pressed, this, &APlayerCharacter::SwapWeapon, static_cast<uint8>(2));

	PlayerInputComponent->BindAction<FIndexer>(TEXT("Attack"), IE_Pressed, this, &APlayerCharacter::PressSkill, static_cast<uint8>(0));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Skill1"), IE_Pressed, this, &APlayerCharacter::PressSkill, static_cast<uint8>(1));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Skill2"), IE_Pressed, this, &APlayerCharacter::PressSkill, static_cast<uint8>(2));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Skill3"), IE_Pressed, this, &APlayerCharacter::PressSkill, static_cast<uint8>(3));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Parrying"), IE_Pressed, this, &APlayerCharacter::PressSkill, static_cast<uint8>(4));

	PlayerInputComponent->BindAction<FIndexer>(TEXT("Attack"), IE_Released, this, &APlayerCharacter::ReleaseSkill, static_cast<uint8>(0));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Skill1"), IE_Released, this, &APlayerCharacter::ReleaseSkill, static_cast<uint8>(1));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Skill2"), IE_Released, this, &APlayerCharacter::ReleaseSkill, static_cast<uint8>(2));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Skill3"), IE_Released, this, &APlayerCharacter::ReleaseSkill, static_cast<uint8>(3));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Parrying"), IE_Released, this, &APlayerCharacter::ReleaseSkill, static_cast<uint8>(4));
}

void APlayerCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::SwapWeapon(uint8 Index)
{
	if (CurWeaponIndex == Index) return;

	if (AWeapon* OldWeapon = GetWeapon())
		OldWeapon->Unequip();

	CurWeaponIndex = Index;
	SetWeapon(Weapons[CurWeaponIndex]);

	if (AWeapon* CurWeapon = GetWeapon())
		CurWeapon->Equip();
}

void APlayerCharacter::SwapWeapon(float Value)
{
	uint8 Index = (CurWeaponIndex + FMath::RoundToInt(Value) + 3) % 3;
	SwapWeapon(Index);
}