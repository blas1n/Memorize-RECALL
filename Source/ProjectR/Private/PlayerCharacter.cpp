// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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
	Unequip = nullptr;
	Energy = 0;
	MaxEnergy = 0;
	EnergyHeal = 0.0f;
	LockOnDistance = 0.0f;
	LockOnAngle = 0.0f;
	LockOnEnemy = nullptr;
	JumpDelay = 0.0f;
	CurWeaponIndex = 0;
	bIsReadyDodge = false;
}

void APlayerCharacter::EquipWeapon(FName Name, uint8 Index)
{
	AWeapon* BeforeWeapon = Weapons[Index];
	Weapons[Index] = Name != TEXT("Unequip") ? GenerateWeapon(Name) : Unequip;

	if (Index == CurWeaponIndex)
		SetWeapon(Weapons[Index]);

	if (BeforeWeapon && BeforeWeapon != Unequip)
		BeforeWeapon->Destroy();
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

void APlayerCharacter::LockOff()
{
	LockOnEnemy->SetLockOn(false);
	LockOnEnemy = nullptr;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	Energy = MaxEnergy;
	OnAttack.AddDynamic(this, &APlayerCharacter::HealEnergyByAttack);
}

void APlayerCharacter::Tick(float DeltaTimes)
{
	Super::Tick(DeltaTimes);

	if (!LockOnEnemy) return;

	const FVector EnemyLocation = LockOnEnemy->GetActorLocation();
	const FVector PlayerLocation = GetActorLocation();
	const float LengthSquare = (EnemyLocation - PlayerLocation).SizeSquared();

	if (LengthSquare > FMath::Square(LoseLockOnDistance))
	{
		LockOff();
		return;
	}

	const FVector CameraLocation = GetFollowCamera()->GetComponentLocation();
	const FRotator LookRotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, EnemyLocation);
	const FRotator NowRotation = FMath::Lerp(GetControlRotation(), LookRotation, DeltaTimes * 5.0f);
	GetController()->SetControlRotation(NowRotation);
}

void APlayerCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (AWeapon* CurWeapon : Weapons)
		if (CurWeapon != Unequip)
			CurWeapon->Destroy();

	Unequip->Destroy();
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerCharacter::AddYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayerCharacter::AddPitchInput);

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

	PlayerInputComponent->BindAction(TEXT("LockOn"), IE_Pressed, this, &APlayerCharacter::LockOn);
}

void APlayerCharacter::CreateWeapons(TArray<FName>&& WeaponNames)
{
	FName UnequipName = TEXT("Unequip");
	Unequip = GenerateWeapon(UnequipName);
	
	const int32 WeaponNum = Weapons.Num();
	
	TArray<FName> UnequipArray;
	UnequipArray.Init(UnequipName, WeaponNum - WeaponNames.Num());
	WeaponNames.Append(MoveTemp(UnequipArray));

	for (uint8 Idx = 0; Idx < WeaponNum; ++Idx)
		EquipWeapon(MoveTemp(WeaponNames[Idx]), Idx);
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

void APlayerCharacter::AddYawInput(float Value)
{
	if (!LockOnEnemy)
		AddControllerYawInput(Value);
}

void APlayerCharacter::AddPitchInput(float Value)
{
	if (!LockOnEnemy)
		AddControllerPitchInput(Value);
}

void APlayerCharacter::PressDodge()
{
	if (IsCasting() || GetCharacterMovement()->IsFalling()) return;
	SetCastData(true, true);
	bIsReadyDodge = true;

	GetWorldTimerManager().SetTimer(DodgeTimer, [this]
	{
		SetCastData();
		bIsReadyDodge = false;
		Jump();
	}, JumpDelay, false);
}

void APlayerCharacter::ReleaseDodge()
{
	if (bIsReadyDodge)
	{
		SetCastData();
		bIsReadyDodge = false;
		GetWorldTimerManager().ClearTimer(DodgeTimer);
		GetWeapon()->Dodge();
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

void APlayerCharacter::LockOn()
{
	if (LockOnEnemy)
	{
		LockOff();
		return;
	}

	TArray<AActor*> Enemys;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyClass, Enemys);

	float Angle = 0.0f, Distance = 0.0f;

	for (AActor* Enemy : Enemys)
		if (CheckLockOn(Enemy, Angle, Distance))
			LockOnEnemy = Cast<AProjectRCharacter>(Enemy);

	if (LockOnEnemy)
		LockOnEnemy->SetLockOn(true);
}

bool APlayerCharacter::CheckLockOn(const AActor* Enemy, float& OutAngle, float& OutDistance) const
{
	if (auto* EnemyChararcter = Cast<AProjectRCharacter>(Enemy))
		if (EnemyChararcter->IsDeath())
			return false;

	const FVector PlayerLocation = GetActorLocation();
	const FVector EnemyLocation = Enemy->GetActorLocation();
	FVector Diff = (EnemyLocation - PlayerLocation);

	const float SizeSquare = Diff.SizeSquared();
	if (SizeSquare > FMath::Square(LockOnDistance)) return false;

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	Diff.Normalize();
	const float Angle = FMath::Acos(FVector::DotProduct(Diff, Direction));

	if (Angle > FMath::DegreesToRadians(LockOnAngle * 0.5f))
		return false;

	FVector PlayerEye; FRotator EyeRot;
	GetActorEyesViewPoint(PlayerEye, EyeRot);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(Enemy);

	FHitResult Hit;
	bool bHaveObstacle = GetWorld()->LineTraceSingleByChannel
		(Hit, PlayerEye, EnemyLocation, ECollisionChannel::ECC_Visibility, Params);

	if (bHaveObstacle) return false;
	if (!LockOnEnemy || Angle < OutAngle)
	{
		OutAngle = Angle;
		OutDistance = SizeSquare;
		return true;
	}

	return false;
}

void APlayerCharacter::HealEnergyByAttack(AProjectRCharacter* Target, int32 Damage)
{
	HealEnergy(Damage * EnergyHeal);
}