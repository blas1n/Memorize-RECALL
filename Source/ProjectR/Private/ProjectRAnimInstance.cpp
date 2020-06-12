// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectRAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AnimCastData.h"
#include "ProjectRCharacter.h"
#include "Weapon.h"

UProjectRAnimInstance::UProjectRAnimInstance()
	: Super()
{
	LocomotionSpace = nullptr;
	JumpStart = nullptr;
	JumpLoop = nullptr;
	JumpEnd = nullptr;
	Speed = 0.0f;
	bIsInAir = false;
}

void UProjectRAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	OnMontageStarted.AddDynamic(this, &UProjectRAnimInstance::OnBeginMontage);
	OnMontageEnded.AddDynamic(this, &UProjectRAnimInstance::OnEndMontage);

	AProjectRCharacter* Owner = Cast<AProjectRCharacter>(TryGetPawnOwner());
	Owner->OnEquipped.AddDynamic(this, &UProjectRAnimInstance::OnEquipped);

	AWeapon* CurWeapon = GetWeapon();
	if (!CurWeapon) return;
	
	FOnAsyncLoadEndedSingle Callback;
	Callback.BindDynamic(this, &UProjectRAnimInstance::OnWeaponAsyncLoaded);
	CurWeapon->RegisterOnAsyncLoadEnded(Callback);
}

void UProjectRAnimInstance::OnEquipped(AWeapon* Weapon)
{
	LocomotionSpace = Weapon->GetLocomotionSpace();
	JumpStart = Weapon->GetJumpStart();
	JumpLoop = Weapon->GetJumpLoop();
	JumpEnd = Weapon->GetJumpEnd();
}

void UProjectRAnimInstance::OnBeginMontage(UAnimMontage* Montage)
{
	AProjectRCharacter* User = Cast<AProjectRCharacter>(TryGetPawnOwner());

	for (const UAnimMetaData* Data : Montage->GetMetaData())
	{
		if (const UAnimCastData* CastData = Cast<UAnimCastData>(Data))
		{
			User->SetCastData(CastData->IsCasting(), CastData->CanMoving());
			break;
		}
	}
}

void UProjectRAnimInstance::OnEndMontage(UAnimMontage* Montage, bool bInterrupted)
{
	Cast<AProjectRCharacter>(TryGetPawnOwner())->SetCastData();
}

void UProjectRAnimInstance::OnWeaponAsyncLoaded()
{
	OnEquipped(Cast<AProjectRCharacter>(TryGetPawnOwner())->GetWeapon());
}

void UProjectRAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ACharacter* Owner = Cast<ACharacter>(TryGetPawnOwner());
	if (!IsValid(Owner)) return;

	Speed = Owner->GetVelocity().Size();
	bIsInAir = Owner->GetCharacterMovement()->IsFalling();
}

void UProjectRAnimInstance::AnimNotify_BeginSkill()
{
	UAnimMontage* Montage = GetCurrentActiveMontage();
	AWeapon* Weapon = GetWeapon();

	if (IsValid(Weapon))
		Weapon->BeginSkill(Montage);
}

void UProjectRAnimInstance::AnimNotify_EndSkill()
{
	UAnimMontage* Montage = GetCurrentActiveMontage();
	OnEndMontage(Montage, false);
	
	AWeapon* Weapon = GetWeapon();

	if (IsValid(Weapon))
		Weapon->EndSkill(Montage, false);
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