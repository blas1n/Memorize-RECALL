// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRPlayerController.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Buff/Lock.h"
#include "Buff/Root.h"
#include "Character/ProjectRCharacter.h"
#include "Character/ProjectRPlayerState.h"
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
	Super::OnUnPossess();
}

void AProjectRPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (!User) return;

	SetUserTransformByInput();
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
	if (!User || IsBuffActivate(URoot::StaticClass())) return;

	if (IsBuffActivate(ULock::StaticClass()))
		User->AddMovementInput(GetDirectionVectorByActor(EAxis::X), Value);
	else
		MoveInput.X = Value;
}

void AProjectRPlayerController::MoveRight(float Value)
{
	if (!User || IsBuffActivate(URoot::StaticClass())) return;

	if (IsBuffActivate(ULock::StaticClass()))
		User->AddMovementInput(GetDirectionVectorByActor(EAxis::Y), Value);
	else
		MoveInput.Y = Value;
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
	if (!User) return;

	if (IsBuffActivate(ULock::StaticClass()))
		User->GetWeaponComponent()->UseSkill(4);
	else User->Jumping();
}

void AProjectRPlayerController::ReleaseDodge()
{
	if (User || User->GetCharacterMovement()->IsFalling())
		User->StopJumping();
}

void AProjectRPlayerController::Run()
{
	if (User) User->Run();
}

void AProjectRPlayerController::Walk()
{
	if (User) User->Walk();
}

void AProjectRPlayerController::SwapWeapon(uint8 Index)
{
	if (User)
		User->GetWeaponComponent()->SwapWeapon(Index);
}

void AProjectRPlayerController::SwapWeapon(float Value)
{
	if (!User || Value == 0.0f) return;

	const auto* WeaponComponent = User->GetWeaponComponent();
	const uint8 Idx = WeaponComponent->GetDeltaWeaponIndex(FMath::RoundFromZero(Value));
	SwapWeapon(Idx);
}

void AProjectRPlayerController::UseSkill(uint8 Index)
{
	if (User)
		User->GetWeaponComponent()->UseSkill(Index);
}

void AProjectRPlayerController::LockOn()
{
	if (!User) return;

	TArray<AActor*> Enemys;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AProjectRCharacter::StaticClass(), Enemys);

	AProjectRCharacter* LockTarget = nullptr;
	float Angle = -1.0f, Distance = -1.0f;

	for (AActor* Enemy : Enemys)
		if (CheckLockOn(Enemy, Angle, Distance))
			LockTarget = Cast<AProjectRCharacter>(Enemy);

	auto* LockBuff = Cast<ULock>(GetPlayerState
		<AProjectRPlayerState>()->GetBuff(ULock::StaticClass()));

	LockBuff->SetLockedTarget(LockTarget);

	if (!LockTarget)
	{
		TurnRotation.Yaw = User->GetActorRotation().Yaw;
		bIsTurning = true;
	}

	LockBuff->ApplyBuff();
}

void AProjectRPlayerController::LockOff()
{
	GetPlayerState<AProjectRPlayerState>()->GetBuff(ULock::StaticClass())->ReleaseBuff();
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
	const float Angle = FMath::Acos(FVector::DotProduct(Diff, GetDirectionVectorByController(EAxis::X)));

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

void AProjectRPlayerController::SetUserTransformByInput()
{
	if (IsBuffActivate(ULock::StaticClass()) || IsBuffActivate(URoot::StaticClass()) || MoveInput.IsZero()) return;

	const FVector Forward = GetDirectionVectorByActor(EAxis::X);
	const float Value = FMath::Max(FMath::Abs(MoveInput.X), FMath::Abs(MoveInput.Y));
	User->AddMovementInput(Forward, Value);

	const FVector Direction = MoveInput.GetUnsafeNormal();
	float Angle = FMath::Acos(FVector::DotProduct(Direction, FVector::ForwardVector));
	const bool IsClockwise = FVector::CrossProduct(Direction, FVector::ForwardVector).Z <= 0.0f;
	Angle = FMath::RadiansToDegrees(IsClockwise ? Angle : -Angle);

	const float Yaw = FRotator::NormalizeAxis(GetControlRotation().Yaw + Angle);
	User->SetTurn(Yaw);
}

void AProjectRPlayerController::CheckLockTarget()
{
	const auto* LockTarget = Cast<ULock>(GetPlayerState
		<AProjectRPlayerState>()->GetBuff(ULock::StaticClass()))->GetLockedTarget();

	if (!IsValid(LockTarget)) return;

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

FVector AProjectRPlayerController::GetDirectionVectorByActor(EAxis::Type Axis) const
{
	const FRotator Rotation = User->GetActorRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	return FRotationMatrix(YawRotation).GetUnitAxis(Axis);
}

FVector AProjectRPlayerController::GetDirectionVectorByController(EAxis::Type Axis) const
{
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	return FRotationMatrix(YawRotation).GetUnitAxis(Axis);
}

bool AProjectRPlayerController::IsBuffActivate(TSubclassOf<class UBuff> BuffClass) const
{
	return GetPlayerState<AProjectRPlayerState>()->GetBuff(BuffClass)->IsActivate();
}
