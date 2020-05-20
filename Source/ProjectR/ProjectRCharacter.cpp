// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ProjectRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

AProjectRCharacter::AProjectRCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

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
}

void AProjectRCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AProjectRCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AProjectRCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction<FSkill>("Skill1", IE_Pressed, this, &AProjectRCharacter::PressSkill, 1);
	PlayerInputComponent->BindAction<FSkill>("Skill2", IE_Pressed, this, &AProjectRCharacter::PressSkill, 2);
	PlayerInputComponent->BindAction<FSkill>("Skill3", IE_Pressed, this, &AProjectRCharacter::PressSkill, 3);

	PlayerInputComponent->BindAction<FSkill>("Skill1", IE_Released, this, &AProjectRCharacter::ReleaseSkill, 1);
	PlayerInputComponent->BindAction<FSkill>("Skill2", IE_Released, this, &AProjectRCharacter::ReleaseSkill, 2);
	PlayerInputComponent->BindAction<FSkill>("Skill3", IE_Released, this, &AProjectRCharacter::ReleaseSkill, 3);
}

void AProjectRCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AProjectRCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AProjectRCharacter::PressSkill(int32 index)
{

}

void AProjectRCharacter::ReleaseSkill(int32 index)
{

}