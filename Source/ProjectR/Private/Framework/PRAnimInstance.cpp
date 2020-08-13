// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PRAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Framework/PRCharacter.h"

UPRAnimInstance::UPRAnimInstance()
	: Super()
{
	Speed = 0.0f;
	bIsLooking = false;
	bIsInAir = false;
}

void UPRAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const auto* User = Cast<APRCharacter>(TryGetPawnOwner());
	if (!User) return;

	const auto* Movement = User->GetCharacterMovement();
	Velocity = User->GetActorRotation().UnrotateVector(Movement->Velocity);
	Speed = Velocity.Size();
	
	bIsLooking = User->IsLocked();
	bIsInAir = Movement->IsFalling();
}
