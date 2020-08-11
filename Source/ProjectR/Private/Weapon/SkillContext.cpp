// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/SkillContext.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/NetDriver.h"
#include "GameFramework/Character.h"

void USkillContext::SetWeaponMesh(UStaticMesh* RightMesh, const FTransform&
	RightTransform, UStaticMesh* LeftMesh, const FTransform& LeftTransform)
{
	MulticastSetWeaponMesh(RightMesh, RightTransform, LeftMesh, LeftTransform);
}

void USkillContext::SetWeaponCollision(bool bEnableRight, bool bEnableLeft)
{
	check(GetTypedOuter<AActor>()->HasAuthority());

	RightWeapon->SetCollisionEnabled(bEnableRight ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	LeftWeapon->SetCollisionEnabled(bEnableLeft ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void USkillContext::PlayAnimation(UAnimMontage* Animation)
{
	auto* User = GetTypedOuter<ACharacter>();
	check(User->HasAuthority() && Animation);

	User->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &USkillContext::OnMontageEnded);
	PlayingMontage = Animation;
	MulticastPlayAnimation(Animation);
}

void USkillContext::StopAnimation()
{
	auto* User = GetTypedOuter<ACharacter>();
	check(User->HasAuthority() && PlayingMontage);

	User->GetMesh()->GetAnimInstance()->OnMontageEnded.RemoveDynamic(this, &USkillContext::OnMontageEnded);
	
	UAnimMontage* CurMontage = PlayingMontage;
	PlayingMontage = nullptr;
	MulticastStopAnimation(CurMontage);
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
	GetTypedOuter<ACharacter>()->StopAnimMontage(PlayingMontage);
}

void USkillContext::MulticastSetWeaponMesh_Implementation(UStaticMesh* RightMesh,
	const FTransform& RightTransform, UStaticMesh* LeftMesh, const FTransform& LeftTransform)
{
	RightWeapon->SetStaticMesh(RightMesh);
	RightWeapon->SetRelativeTransform(RightTransform);

	LeftWeapon->SetStaticMesh(LeftMesh);
	LeftWeapon->SetRelativeTransform(LeftTransform);
}

void USkillContext::Initialize(UStaticMeshComponent* InRightWeapon, class UStaticMeshComponent* InLeftWeapon)
{
	RightWeapon = InRightWeapon;
	LeftWeapon = InLeftWeapon;

	auto* Outer = GetTypedOuter<AActor>();
	if (Outer && Outer->HasAuthority())
	{
		RightWeapon->OnComponentBeginOverlap.AddUniqueDynamic(this, &USkillContext::OnWeaponOverlap);
		LeftWeapon->OnComponentBeginOverlap.AddUniqueDynamic(this, &USkillContext::OnWeaponOverlap);
		SetWeaponCollision(false, false);
	}
	else
	{
		RightWeapon->SetCollisionProfileName(TEXT("NoCollision"));
		LeftWeapon->SetCollisionProfileName(TEXT("NoCollision"));

		RightWeapon->SetGenerateOverlapEvents(false);
		LeftWeapon->SetGenerateOverlapEvents(false);
	}
}

	return false;
}
