// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Cast.h"

bool UCast::CanUseSkill(const USkill* Skill) const
{
	return !IsActivate() || CurSkill->GetPriority() < Skill->GetPriority();
}

void UCast::OnApply()
{
	bIsCasting = true;
}

void UCast::OnRelease()
{
	bIsCasting = false;
}

bool UCast::IsActivate_Implementation() const
{
	return bIsCasting;
}
