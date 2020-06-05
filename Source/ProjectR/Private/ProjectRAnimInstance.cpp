// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectRAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectRCharacter.h"
#include "Weapon.h"

UProjectRAnimInstance::UProjectRAnimInstance()
{
	Speed = 0.0f;
	IsInAir = false;
}

void UProjectRAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ACharacter* Owner = Cast<ACharacter>(TryGetPawnOwner());
	if (!IsValid(Owner)) return;

	Speed = Owner->GetVelocity().Size();
	IsInAir = Owner->GetCharacterMovement()->IsFalling();
}

void UProjectRAnimInstance::AnimNotify_BeginAttack()
{
	AWeapon* Weapon = GetWeapon();
	
	if (IsValid(Weapon))
		Weapon->OnBeginAttack.Broadcast();
}

void UProjectRAnimInstance::AnimNotify_EndAttack()
{
	AWeapon* Weapon = GetWeapon();
	
	if (IsValid(Weapon))
		Weapon->OnEndAttack.Broadcast();
}

void UProjectRAnimInstance::AnimNotify_Shoot()
{
	AWeapon* Weapon = GetWeapon();
	
	if (IsValid(Weapon))
		Weapon->OnShoot.Broadcast();
}

void UProjectRAnimInstance::AnimNotify_Execute()
{
	AWeapon* Weapon = GetWeapon();
	
	if (IsValid(Weapon))
		Weapon->OnExecute.Broadcast();
}

AWeapon* UProjectRAnimInstance::GetWeapon() const
{
	AProjectRCharacter* Character = Cast<AProjectRCharacter>(TryGetPawnOwner());
	return IsValid(Character) ? Character->GetWeapon() : nullptr;
}