// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/SkillContext.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/NetDriver.h"
#include "GameFramework/Character.h"

void USkillContext::Initialize(UStaticMeshComponent* InRightMeshComp, UStaticMeshComponent* InLeftMeshComp)
{
	RightWeaponComp = InRightMeshComp;
	LeftWeaponComp = InLeftMeshComp;

	auto* Outer = GetTypedOuter<AActor>();
	if (Outer && Outer->HasAuthority())
	{
		RightWeaponComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &USkillContext::OnWeaponOverlap);
		LeftWeaponComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &USkillContext::OnWeaponOverlap);
		SetWeaponCollision(false, false);
	}
	else
	{
		RightWeaponComp->SetCollisionProfileName(TEXT("NoCollision"));
		LeftWeaponComp->SetCollisionProfileName(TEXT("NoCollision"));

		RightWeaponComp->SetGenerateOverlapEvents(false);
		LeftWeaponComp->SetGenerateOverlapEvents(false);
	}
}

void USkillContext::SetWeaponCollision(bool bEnableRight, bool bEnableLeft)
{
	check(GetTypedOuter<AActor>()->HasAuthority());

	RightWeaponComp->SetCollisionEnabled(bEnableRight ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	LeftWeaponComp->SetCollisionEnabled(bEnableLeft ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
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
	if (PlayingMontage == Montage)
	{
		OnAnimationEnded.Broadcast();
		PlayingMontage = nullptr;
	}
}

void USkillContext::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnWeaponOverlapped.Broadcast(OtherActor);
}

void USkillContext::MulticastPlayAnimation_Implementation(UAnimMontage* Animation)
{
	GetTypedOuter<ACharacter>()->PlayAnimMontage(Animation);
}

void USkillContext::MulticastStopAnimation_Implementation(UAnimMontage* Animation)
{
	GetTypedOuter<ACharacter>()->StopAnimMontage(Animation);
}
