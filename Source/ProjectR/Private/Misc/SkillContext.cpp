// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/SkillContext.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

void USkillContext::Initialize(const TArray<UPrimitiveComponent*>& InComponents)
{
	auto* User = GetTypedOuter<ACharacter>();
	check(User && User->HasAuthority());
	
	User->GetMesh()->GetAnimInstance()->OnMontageEnded
		.AddUniqueDynamic(this, &USkillContext::OnMontageEnded);

	Components = InComponents;
	for (auto* Component : Components)
	{
		Component->SetGenerateOverlapEvents(false);
		Component->OnComponentBeginOverlap.AddUniqueDynamic(this, &USkillContext::OnOverlap);
	}
}

void USkillContext::PlayAnimation(UAnimMontage* Animation, const FOnAnimationEnded& OnAnimationEnded)
{
	check(GetTypedOuter<AActor>()->HasAuthority() && Animation && OnAnimationEnded.IsBound());

	Callbacks.Add(Animation, OnAnimationEnded);
	MulticastPlayAnimation(Animation);
}

void USkillContext::StopAnimation(UAnimMontage* Animation)
{
	check(GetTypedOuter<AActor>()->HasAuthority() && Animation);

	Callbacks.Remove(Animation);
	MulticastStopAnimation(Animation);
}

void USkillContext::SetCollision(int32 AttackPart)
{
	const int32 Num = Components.Num();
	for (int32 Idx = 0; Idx < Num; ++Idx)
	{
		const bool bIsEnable = AttackPart & (1 << Idx);
		Components[Idx]->SetGenerateOverlapEvents(bIsEnable);
	}
}

void USkillContext::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetTypedOuter<AActor>() != OtherActor)
		OnHit.Broadcast(OtherActor);
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
