// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/Skill.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"

void USkill::Initialize(USkillContext* InContext, UDataAsset* Data)
{
	User = GetTypedOuter<APRCharacter>();
	Context = InContext;

	ReceiveInitialize(Data);
}

void USkill::Begin()
{
	if (bIsExecute)
		return;

	bIsExecute = true;
	ReceiveBegin();
}

void USkill::End()
{
	if (!bIsExecute)
		return;

	bIsExecute = false;
	ReceiveEnd();

	User->GetWeaponComponent()->OnEndSkill();
}

UWorld* USkill::GetWorld() const
{
	return User ? User->GetWorld() : nullptr;
}
