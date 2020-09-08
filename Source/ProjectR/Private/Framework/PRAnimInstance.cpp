// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PRAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"

UPRAnimInstance::UPRAnimInstance()
	: Super()
{
	bIsLocking = false;
	bIsInAir = false;
}

void UPRAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const auto* User = Cast<APRCharacter>(GetOwningActor());
	if (!User) return;

	AnimData = User->GetWeaponComponent()->GetAnimData();

	const auto* Movement = User->GetCharacterMovement();
	Velocity = FVector2D{ User->GetActorRotation().UnrotateVector(Movement->Velocity) };

	bIsLocking = User->IsLocked();
	bIsInAir = Movement->IsFalling();
}
