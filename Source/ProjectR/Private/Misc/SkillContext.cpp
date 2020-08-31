// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/SkillContext.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

void USkillContext::Initialize(UStaticMeshComponent* InRightWeapon, UStaticMeshComponent* InLeftWeapon)
{
	RightWeapon = InRightWeapon;
	LeftWeapon = InLeftWeapon;
}

void USkillContext::PlayAnimation(UAnimMontage* Animation)
{
	auto* User = GetTypedOuter<ACharacter>();
	check(User->HasAuthority() && Animation);

	User->GetMesh()->GetAnimInstance()->OnMontageEnded.AddUniqueDynamic(this, &USkillContext::OnMontageEnded);
	MulticastPlayAnimation(Animation);
}

void USkillContext::StopAnimation(UAnimMontage* Animation)
{
	auto* User = GetTypedOuter<ACharacter>();
	check(User->HasAuthority() && Animation);

	User->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &USkillContext::OnMontageEnded);
	MulticastStopAnimation(Animation);
}

void USkillContext::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
		OnAnimationEnded.Broadcast();
}

void USkillContext::MulticastPlayAnimation_Implementation(UAnimMontage* Animation)
{
	GetTypedOuter<ACharacter>()->PlayAnimMontage(Animation);
}

void USkillContext::MulticastStopAnimation_Implementation(UAnimMontage* Animation)
{
	GetTypedOuter<ACharacter>()->StopAnimMontage(Animation);
}
