// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Skill.h"
#include "Framework/PRCharacter.h"

void USkill::Initialize(UWeaponContext* InContext, UObject* Data)
{
	User = GetTypedOuter<APRCharacter>();
	Context = InContext;

	ReceiveInitialize(Data);
}

void USkill::Begin()
{
	if (!bIsExecute)
		ReceiveBegin();

	bIsExecute = true;
}

void USkill::End()
{
	if (bIsExecute)
		ReceiveEnd();

	bIsExecute = false;
}
