// Fill out your copyright notice in the Description page of Project Settings.

#include "Skill/SingleAttack.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Data/SingleAttackData.h"
#include "Framework/PRCharacter.h"
#include "Misc/SkillContext.h"

void USingleAttack::Begin(USkillContext* InContext, const UDataAsset* Data)
{
	Super::Begin(InContext, Data);

	if (!IsValidInput(InContext, Data))
	{
		Context = nullptr;
		Finish();
		return;
	}

	Context = InContext;

	const auto* MyData = Cast<const USingleAttackData>(Data);
	
	Damage = MyData->Damage;
	AttackPart = MyData->AttackPart;
	Animation = MyData->Animation;

	Context->OnHit.AddDynamic(this, &USingleAttack::OnHit);

	FOnAnimationEnded Callback;
	Callback.BindDynamic(this, &USingleAttack::OnAnimationEnded);
	Context->PlayAnimation(Animation, Callback);
}

void USingleAttack::End()
{
	if (!Context)
	{
		Super::End();
		return;
	}

	Context->OnHit.RemoveDynamic(this, &USingleAttack::OnHit);
	Context->StopAnimation(Animation);

	Super::End();
}

bool USingleAttack::CanUseSkill_Implementation() const
{
	return !GetUser()->GetCharacterMovement()->IsFalling();
}

void USingleAttack::BeginExecute_Implementation()
{
	Context->SetCollision(AttackPart);

	AttackedActors.Empty();
	AttackedActors.Add(GetUser());
}

void USingleAttack::EndExecute_Implementation()
{
	Context->SetCollision(0);
}

bool USingleAttack::IsValidInput(USkillContext* InContext, const UDataAsset* Data) const
{
	if (!InContext)	return false;

	const auto* MyData = Cast<const USingleAttackData>(Data);
	if (!MyData) return false;

	return MyData->AttackPart != 0 && MyData->Animation != nullptr;
}

void USingleAttack::OnHit(AActor* Target)
{
	if (AttackedActors.Contains(Target)) return;

	AttackedActors.Add(Target);
	Target->TakeDamage(Damage, FDamageEvent{},
		GetUser()->GetController(), GetUser());
}

void USingleAttack::OnAnimationEnded()
{
	Finish();
}
