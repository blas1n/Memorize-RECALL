// Fill out your copyright notice in the Description page of Project Settings.

#include "Skill/Skill.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"

void USkill::Initialize()
{
	User = GetTypedOuter<APRCharacter>();
	ReceiveInitialize();
}

void USkill::Begin(USkillContext* Context, const UDataAsset* Data)
{
	ReceiveBegin(Context, Data);
}

void USkill::Tick(float DeltaTime)
{
	ReceiveTick(DeltaTime);
}

void USkill::End()
{
	ReceiveEnd();
	User->GetWeaponComponent()->OnEndSkill();
}

void USkill::Finish()
{
	End();
}

UWorld* USkill::GetWorld() const
{
	return User ? User->GetWorld() : nullptr;
}
