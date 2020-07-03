// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Buff/Lock.h"
#include "Buff/Root.h"
#include "Character/ProjectRCharacter.h"
#include "Character/ProjectRPlayerState.h"
#include "Weapon.h"
#include "WeaponComponent.h"

UProjectRAnimInstance::UProjectRAnimInstance()
	: Super()
{
	User = nullptr;
	Speed = 0.0f;
	bIsLooking = false;
	bIsInAir = false;
}

void UProjectRAnimInstance::NativeBeginPlay()
{
	User = Cast<AProjectRCharacter>(TryGetPawnOwner());
	Super::NativeBeginPlay();	
}

void UProjectRAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!IsValid(User)) return;

	const auto* Movement = User->GetCharacterMovement();
	Velocity = User->GetActorRotation().UnrotateVector(Movement->Velocity);
	Speed = Velocity.Size();
	
	bIsLooking = User->GetPlayerState<AProjectRPlayerState>()
		->GetBuff(ULock::StaticClass())->IsActivate();

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

void UProjectRAnimInstance::AnimNotify_EnableMove()
{
	User->GetPlayerState<AProjectRPlayerState>()->GetBuff(URoot::StaticClass())->ReleaseBuff();
}

void UProjectRAnimInstance::AnimNotify_DisableMove()
{
	User->GetPlayerState<AProjectRPlayerState>()->GetBuff(URoot::StaticClass())->ApplyBuff();
}

UWeapon* UProjectRAnimInstance::GetWeapon() const
{
	return User->GetWeaponComponent()->GetWeapon();
}
