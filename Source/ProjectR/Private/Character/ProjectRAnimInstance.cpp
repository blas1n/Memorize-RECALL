// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Buff/Faint.h"
#include "Buff/Lock.h"
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
	bIsStuned = UBuffLibrary::IsActivate<UFaint>(User);
	bIsInAir = Movement->IsFalling();
}

void UProjectRAnimInstance::AnimNotify_BeginAttack()
{
	GetWeapon()->OnBeginAttack.Broadcast();
}

void UProjectRAnimInstance::AnimNotify_EndAttack()
{
	GetWeapon()->OnEndAttack.Broadcast();
}

void UProjectRAnimInstance::AnimNotify_Shoot()
{
	GetWeapon()->OnShoot.Broadcast();
}

void UProjectRAnimInstance::AnimNotify_Execute()
{
	GetWeapon()->OnExecute.Broadcast();
}

UWeapon* UProjectRAnimInstance::GetWeapon() const
{
	const auto* User = Cast<AProjectRCharacter>(TryGetPawnOwner());
	return User ? User->GetWeaponComponent()->GetWeapon() : nullptr;
}
