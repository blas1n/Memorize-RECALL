// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRPlayerController.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Buff/Lock.h"
#include "Buff/Root.h"
#include "Buff/Run.h"
#include "Character/ProjectRCharacter.h"
#include "BuffLibrary.h"
#include "WeaponComponent.h"

DECLARE_DELEGATE_OneParam(FIndexer, uint8);

void AProjectRPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	User = Cast<AProjectRCharacter>(InPawn);
}

void AProjectRPlayerController::OnUnPossess()
{
	User = nullptr;

	if (GetPawn() != nullptr)
	{
		Cast<AProjectRCharacter>(GetPawn())->OnDeath
			.RemoveDynamic(this, &AProjectRPlayerController::OnDeath);
	}

	Super::OnUnPossess();
}

void AProjectRPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (!User) return;

	CheckLockTarget();
	Turn(DeltaSeconds);
}

void AProjectRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis(TEXT("MoveForward"), this, &AProjectRPlayerController::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &AProjectRPlayerController::MoveRight);

	InputComponent->BindAxis(TEXT("Turn"), this, &AProjectRPlayerController::InputYaw);
	InputComponent->BindAxis(TEXT("LookUp"), this, &AProjectRPlayerController::InputPitch);

	InputComponent->BindAxis(TEXT("Swap"), this, &AProjectRPlayerController::SwapWeapon);

	InputComponent->BindAction(TEXT("Dodge"), IE_Pressed, this, &AProjectRPlayerController::PressDodge);
	InputComponent->BindAction(TEXT("Dodge"), IE_Released, this, &AProjectRPlayerController::ReleaseDodge);

	InputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &AProjectRPlayerController::Run);
	InputComponent->BindAction(TEXT("Run"), IE_Released, this, &AProjectRPlayerController::Walk);

	InputComponent->BindAction<FIndexer>(TEXT("Weapon1"), IE_Pressed, this, &AProjectRPlayerController::SwapWeapon, static_cast<uint8>(0));
	InputComponent->BindAction<FIndexer>(TEXT("Weapon2"), IE_Pressed, this, &AProjectRPlayerController::SwapWeapon, static_cast<uint8>(1));
	InputComponent->BindAction<FIndexer>(TEXT("Weapon3"), IE_Pressed, this, &AProjectRPlayerController::SwapWeapon, static_cast<uint8>(2));

	InputComponent->BindAction<FIndexer>(TEXT("Attack"), IE_Pressed, this, &AProjectRPlayerController::UseSkill, static_cast<uint8>(0));
	InputComponent->BindAction<FIndexer>(TEXT("Skill1"), IE_Pressed, this, &AProjectRPlayerController::UseSkill, static_cast<uint8>(1));
	InputComponent->BindAction<FIndexer>(TEXT("Skill2"), IE_Pressed, this, &AProjectRPlayerController::UseSkill, static_cast<uint8>(2));
	InputComponent->BindAction<FIndexer>(TEXT("Skill3"), IE_Pressed, this, &AProjectRPlayerController::UseSkill, static_cast<uint8>(3));
	InputComponent->BindAction<FIndexer>(TEXT("Parrying"), IE_Pressed, this, &AProjectRPlayerController::UseSkill, static_cast<uint8>(4));

	InputComponent->BindAction(TEXT("Lock"), IE_Pressed, this, &AProjectRPlayerController::LockOn);
	InputComponent->BindAction(TEXT("Lock"), IE_Released, this, &AProjectRPlayerController::LockOff);
}

void AProjectRPlayerController::MoveForward(float Value)
{
	if (User && !UBuffLibrary::IsActivate<URoot>(User))
		User->AddMovementInput(GetDirectionVector(EAxis::X), Value);
}

void AProjectRPlayerController::MoveRight(float Value)
{
	if (User && !UBuffLibrary::IsActivate<URoot>(User))
		User->AddMovementInput(GetDirectionVector(EAxis::Y), Value);
}

void AProjectRPlayerController::InputYaw(float Value)
{
	if (!bIsTurning)
		AddYawInput(Value);
}

void AProjectRPlayerController::InputPitch(float Value)
{
	if (!bIsTurning)
		AddPitchInput(Value);
}

void AProjectRPlayerController::PressDodge()
{
	if (!User || UBuffLibrary::IsActivate<URoot>(User)) return;

	if (UBuffLibrary::IsActivate<ULock>(User))
		User->GetWeaponComponent()->StartSkill(4);
	else User->Jump();
}

void AProjectRPlayerController::ReleaseDodge()
{
	if (User && User->GetCharacterMovement()->IsFalling())
		User->StopJumping();
}

void AProjectRPlayerController::Run()
{
	UBuffLibrary::ApplyBuff<URun>(User);
	bIsRunned = true;

	auto Temp = bIsLocked;
	if (Temp) LockOff();
	bIsLocked = MoveTemp(Temp);
}

void AProjectRPlayerController::Walk()
{
	UBuffLibrary::ReleaseBuff<URun>(User);
	bIsRunned = false;
	if (bIsLocked) LockOn();
}

void AProjectRPlayerController::SwapWeapon(uint8 Index)
{
	if (User) User->GetWeaponComponent()->SwapWeapon(Index);
}

void AProjectRPlayerController::SwapWeapon(float Value)
{
	if (!User || Value == 0.0f) return;

	const auto* WeaponComponent = User->GetWeaponComponent();
	const int32 WeaponNum = static_cast<int32>(WeaponComponent->GetWeaponNum());

	int32 Idx = static_cast<int32>(WeaponComponent->GetWeaponIndex());
	Idx += FMath::RoundFromZero(Value);
	Idx = ((Idx % WeaponNum) + WeaponNum) % WeaponNum;
	
	SwapWeapon(static_cast<uint8>(Idx));
}

void AProjectRPlayerController::UseSkill(uint8 Index)
{
	if (User) User->GetWeaponComponent()->StartSkill(Index);
}

void AProjectRPlayerController::LockOn()
{
	if (!User || UBuffLibrary::IsBlocked<ULock>(User)) return;

	TArray<AActor*> Enemys;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProjectRCharacter::StaticClass(), Enemys);

	AProjectRCharacter* LockTarget = nullptr;
	float Angle = -1.0f, Distance = -1.0f;

	for (AActor* Enemy : Enemys)
		if (CheckLockOn(Enemy, Angle, Distance))
			LockTarget = Cast<AProjectRCharacter>(Enemy);

	auto* LockBuff = UBuffLibrary::GetBuff<ULock>(User);
	LockBuff->Lock(LockTarget);

	if (!LockTarget)
	{
		TurnRotation.Yaw = User->GetActorRotation().Yaw;
		bIsTurning = true;
	}

	bIsLocked = true;

	auto Temp = bIsRunned;
	if (Temp) Walk();
	bIsRunned = MoveTemp(Temp);
}

void AProjectRPlayerController::LockOff()
{
	UBuffLibrary::ReleaseBuff<ULock>(User);
	bIsLocked = false;
	if (bIsRunned) Run();
}

bool AProjectRPlayerController::CheckLockOn(const AActor* Enemy, float& OutAngle, float& OutDistance) const
{
	if (auto* EnemyChararcter = Cast<AProjectRCharacter>(Enemy))
		if (EnemyChararcter->IsDeath())
			return false;

	const FVector UserLocation = User->GetActorLocation();
	const FVector EnemyLocation = Enemy->GetActorLocation();
	FVector Diff = (EnemyLocation - UserLocation);

	const float SizeSquare = Diff.SizeSquared();
	if (SizeSquare > FMath::Square(LockOnDistance)) return false;

	Diff.Normalize();
	const float Angle = FMath::Acos(FVector::DotProduct(Diff, GetDirectionVector(EAxis::X)));

	if (Angle > FMath::DegreesToRadians(LockOnAngle * 0.5f))
		return false;

	FVector UserEye; FRotator EyeRot;
	GetActorEyesViewPoint(UserEye, EyeRot);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(User);
	Params.AddIgnoredActor(Enemy);

	FHitResult Hit;
	bool bHaveObstacle = GetWorld()->LineTraceSingleByChannel
		(Hit, UserEye, EnemyLocation, ECollisionChannel::ECC_Visibility, Params);

	if (bHaveObstacle) return false;
	if (OutDistance <= 0.0f && OutAngle <= 0.0f)
	{
		OutAngle = Angle;
		OutDistance = SizeSquare;
		return true;
	}

	const float AllowAngle = FMath::DegreesToRadians(LockOnAngle * 0.05f);
	const float AngleDiff = OutAngle - Angle;

	if (FMath::Abs(AngleDiff) > AllowAngle)
	{
		if (AngleDiff > 0.0f)
		{
			OutAngle = Angle;
			OutDistance = SizeSquare;
			return true;
		}

		return false;
	}

	if (OutDistance > SizeSquare)
	{
		OutAngle = Angle;
		OutDistance = SizeSquare;
		return true;
	}

	return false;
}

void AProjectRPlayerController::CheckLockTarget()
{
	const auto* LockTarget = Cast<AProjectRCharacter>(UBuffLibrary::GetBuff<ULock>(User)->GetLockedTarget());
	if (!LockTarget) return;

	if (LockTarget->IsDeath())
	{
		LockOn();
		return;
	}

	const FVector UserLocation = User->GetActorLocation();
	const FVector TargetLocation = LockTarget->GetActorLocation();
	const float LengthSquare = (TargetLocation - UserLocation).SizeSquared();

	if (LengthSquare > FMath::Square(LoseLockOnDistance))
		LockOff();
}

void AProjectRPlayerController::Turn(float DeltaSeconds)
{
	if (!bIsTurning) return;

	const FRotator CurRotation = GetControlRotation();
	if (CurRotation.Equals(TurnRotation, 5.0f))
	{
		bIsTurning = false;
		return;
	}

	SetControlRotation(FMath::Lerp(CurRotation, TurnRotation, DeltaSeconds * 8.0f));
}

FVector AProjectRPlayerController::GetDirectionVector(EAxis::Type Axis) const
{
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	return FRotationMatrix(YawRotation).GetUnitAxis(Axis);
}
