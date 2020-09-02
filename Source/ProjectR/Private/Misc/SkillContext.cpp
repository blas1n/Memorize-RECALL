// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/SkillContext.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

void USkillContext::Initialize(UStaticMeshComponent* InRightWeapon, UStaticMeshComponent* InLeftWeapon)
{
	RightWeapon = InRightWeapon;
	LeftWeapon = InLeftWeapon;

	User = GetTypedOuter<ACharacter>();
	check(User);

	if (User->HasAuthority())
	{
		User->GetMesh()->GetAnimInstance()->OnMontageEnded
			.AddUniqueDynamic(this, &USkillContext::OnMontageEnded);
	}
}

void USkillContext::PlayAnimation(UAnimMontage* Animation, const FOnAnimationEnded& OnAnimationEnded)
{
	check(User->HasAuthority() && Animation && OnAnimationEnded.IsBound());

	Callbacks.Add(Animation, OnAnimationEnded);
	MulticastPlayAnimation(Animation);
}

void USkillContext::StopAnimation(UAnimMontage* Animation)
{
	check(User->HasAuthority() && Animation);

	Callbacks.Remove(Animation);
	MulticastStopAnimation(Animation);
}

void USkillContext::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted) return;

	FOnAnimationEnded Callback;
	if (Callbacks.RemoveAndCopyValue(Montage, Callback))
		Callback.Execute();
}

void USkillContext::MulticastPlayAnimation_Implementation(UAnimMontage* Animation)
{
	GetTypedOuter<ACharacter>()->PlayAnimMontage(Animation);
}

void USkillContext::MulticastStopAnimation_Implementation(UAnimMontage* Animation)
{
	GetTypedOuter<ACharacter>()->StopAnimMontage(Animation);
}
