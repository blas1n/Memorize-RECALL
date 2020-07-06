// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Buff/Faint.h"
#include "Buff/Lock.h"
#include "Buff/Root.h"
#include "Character/ProjectRCharacter.h"
#include "Character/ProjectRPlayerState.h"
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
	
	auto* PlayerState = User->GetPlayerState<AProjectRPlayerState>();
	if (!PlayerState) return;

	bIsLooking = PlayerState->GetBuff(ULock::StaticClass())->IsActivate();
	bIsStuned = PlayerState->GetBuff(UFaint::StaticClass())->IsActivate();
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
	const auto* User = Cast<AProjectRCharacter>(TryGetPawnOwner());
	User->GetPlayerState<AProjectRPlayerState>()->GetBuff(URoot::StaticClass())->ReleaseBuff();
}

void UProjectRAnimInstance::AnimNotify_DisableMove()
{
	const auto* User = Cast<AProjectRCharacter>(TryGetPawnOwner());
	User->GetPlayerState<AProjectRPlayerState>()->GetBuff(URoot::StaticClass())->ApplyBuff();
}

UWeapon* UProjectRAnimInstance::GetWeapon() const
{
	const auto* User = Cast<AProjectRCharacter>(TryGetPawnOwner());
	return User->GetWeaponComponent()->GetWeapon();
}
