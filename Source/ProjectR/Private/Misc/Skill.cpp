// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/Skill.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"

void USkill::Initialize()
{
	User = GetTypedOuter<APRCharacter>();
	ReceiveInitialize();
}

void USkill::Begin(USkillContext* Context, UDataAsset* Data)
{
	ReceiveBegin(Context, Data);
}

void USkill::End()
{
	ReceiveEnd();
	User->GetWeaponComponent()->OnEndSkill();
}

UWorld* USkill::GetWorld() const
{
	return User ? User->GetWorld() : nullptr;
}
