// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Buff/Stun.h"
#include "Buff/Lock.h"
#include "Buff/Root.h"
#include "Character/ProjectRCharacter.h"
#include "BuffLibrary.h"
#include "Weapon.h"
#include "WeaponComponent.h"

UProjectRAnimInstance::UProjectRAnimInstance()
	: Super()
{
	Speed = 0.0f;
	bIsLooking = false;
	bIsStuned = false;
	bIsInAir = false;
}

void UProjectRAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const auto* User = Cast<AProjectRCharacter>(TryGetPawnOwner());
	if (!User) return;

	const auto* Movement = User->GetCharacterMovement();
	Velocity = User->GetActorRotation().UnrotateVector(Movement->Velocity);
	Speed = Velocity.Size();
	
	bIsLooking = UBuffLibrary::IsActivate<ULock>(User);
	bIsStuned = UBuffLibrary::IsActivate<UStun>(User);
	bIsInAir = Movement->IsFalling();
}

void UProjectRAnimInstance::AnimNotify_Execute()
{
	const auto* User = Cast<AProjectRCharacter>(TryGetPawnOwner());
	if (!User) return;

	auto* Weapon = User->GetWeaponComponent()->GetWeapon();
	if (!Weapon) return;

	Weapon->OnExecute.Broadcast();
}

void UProjectRAnimInstance::AnimNotify_EnableMove()
{
	if (auto* User = Cast<AProjectRCharacter>(TryGetPawnOwner()))
		UBuffLibrary::ReleaseBuff<URoot>(User);
}

void UProjectRAnimInstance::AnimNotify_DisableMove()
{
	if (auto* User = Cast<AProjectRCharacter>(TryGetPawnOwner()))
		UBuffLibrary::ApplyBuff<URoot>(User);
}
