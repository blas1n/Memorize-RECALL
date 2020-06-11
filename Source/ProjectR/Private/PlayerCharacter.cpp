// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"
#include "Weapon.h"

DECLARE_DELEGATE_OneParam(FIndexer, uint8);
DECLARE_DELEGATE_OneParam(FSpeedSetter, float);

APlayerCharacter::APlayerCharacter()
	: Super()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Weapons.SetNum(3);
	Energy = 0;
	MaxEnergy = 0;
	EnergyHeal = 0.0f;
	JumpDelay = 0.0f;
	CurWeaponIndex = 0;
	bIsReadyDodge = false;
}

void APlayerCharacter::EquipWeapon(FName Name, uint8 Index)
{
	Weapons[Index] = GenerateWeapon(Name);
}

int32 APlayerCharacter::HealEnergy(int32 Value)
{
	Energy = FMath::Clamp(Energy + Value, 0, MaxEnergy);
	return Energy;
}

int32 APlayerCharacter::SetMaxEnergy(int32 NewMaxEnergy)
{
	int32 Diff = NewMaxEnergy - MaxEnergy;
	MaxEnergy = NewMaxEnergy;
	Energy += Diff;
	return Diff;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Energy = MaxEnergy;
	OnAttack.AddDynamic(this, &APlayerCharacter::HealEnergyByAttack);

	for (AWeapon* CurWeapon : Weapons)
		CurWeapon = GetUnequip();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAxis(TEXT("Swap"), this, &APlayerCharacter::SwapWeapon);

	PlayerInputComponent->BindAction(TEXT("Dodge"), IE_Pressed, this, &APlayerCharacter::PressDodge);
	PlayerInputComponent->BindAction(TEXT("Dodge"), IE_Released, this, &APlayerCharacter::ReleaseDodge);

	PlayerInputComponent->BindAction<FSpeedSetter>(TEXT("Walk"), IE_Pressed, this, &APlayerCharacter::SetSpeed, GetWalkSpeed());
	PlayerInputComponent->BindAction<FSpeedSetter>(TEXT("Walk"), IE_Released, this, &APlayerCharacter::SetSpeed, GetRunSpeed());

	PlayerInputComponent->BindAction<FIndexer>(TEXT("Weapon1"), IE_Pressed, this, &APlayerCharacter::SwapWeapon, static_cast<uint8>(0));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Weapon2"), IE_Pressed, this, &APlayerCharacter::SwapWeapon, static_cast<uint8>(1));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Weapon3"), IE_Pressed, this, &APlayerCharacter::SwapWeapon, static_cast<uint8>(2));

	PlayerInputComponent->BindAction<FIndexer>(TEXT("Attack"), IE_Pressed, this, &APlayerCharacter::UseSkill, static_cast<uint8>(0));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Skill1"), IE_Pressed, this, &APlayerCharacter::UseSkill, static_cast<uint8>(1));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Skill2"), IE_Pressed, this, &APlayerCharacter::UseSkill, static_cast<uint8>(2));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Skill3"), IE_Pressed, this, &APlayerCharacter::UseSkill, static_cast<uint8>(3));
	PlayerInputComponent->BindAction<FIndexer>(TEXT("Parrying"), IE_Pressed, this, &APlayerCharacter::UseSkill, static_cast<uint8>(4));
}

void APlayerCharacter::MoveForward(float Value)
{
	if (!CanMoving() || !Controller || Value == 0.0f) return;

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);
}

void APlayerCharacter::MoveRight(float Value)
{
	if (!CanMoving() || !Controller || Value == 0.0f) return;

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
}

void APlayerCharacter::PressDodge()
{
	if (IsCasting()) return;
	SetIsCasting(true);

	bIsReadyDodge = true;

	GetWorldTimerManager().SetTimer(DodgeTimer, [this]
		{
			Jump();
			bIsReadyDodge = false;
			SetIsCasting(false);
		}, JumpDelay, false);
}

void APlayerCharacter::ReleaseDodge()
{
	if (bIsReadyDodge)
	{
		GetWorldTimerManager().ClearTimer(DodgeTimer);
		PlayAnimMontage(RollAnimMontage);
		bIsReadyDodge = false;
		SetIsCasting(false);
	}
	else if (GetCharacterMovement()->IsFalling())
		StopJumping();
}

void APlayerCharacter::SwapWeapon(uint8 Index)
{
	if (IsCasting() || CurWeaponIndex == Index) return;

	CurWeaponIndex = Index;
	SetWeapon(Weapons[CurWeaponIndex]);
}

void APlayerCharacter::SwapWeapon(float Value)
{
	uint8 Index = (CurWeaponIndex + FMath::RoundToInt(Value) + 3) % 3;
	SwapWeapon(Index);
}

void APlayerCharacter::HealEnergyByAttack(AProjectRCharacter* Target, int32 Damage)
{
	HealEnergy(Damage * EnergyHeal);
}