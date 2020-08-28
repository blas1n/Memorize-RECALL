// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/SkillContext.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"

void USkillContext::Initialize(UStaticMeshComponent* InRightWeapon, UStaticMeshComponent* InLeftWeapon)
{
	RightWeapon = InRightWeapon;
	LeftWeapon = InLeftWeapon;

	RightWeapon->OnComponentBeginOverlap.AddDynamic(this, &USkillContext::OnWeaponOverlapped);
	LeftWeapon->OnComponentBeginOverlap.AddDynamic(this, &USkillContext::OnWeaponOverlapped);
}

void USkillContext::SetWeaponCollision(bool bEnableRight, bool bEnableLeft)
{
	RightWeapon->SetCollisionEnabled(bEnableRight ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	LeftWeapon->SetCollisionEnabled(bEnableLeft ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
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

void USkillContext::OnWeaponOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnHit.Broadcast(OtherActor);
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
