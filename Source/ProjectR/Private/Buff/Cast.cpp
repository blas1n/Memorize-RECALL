// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Cast.h"
#include "Skill.h"

void UCast::StopCast()
{
	if (IsActivate())
		Skill->End();
}

void UCast::CastSkill(USkill* NewSkill)
{
	StopCast();
	Skill = NewSkill;
	Apply();
}

bool UCast::CanUseSkill(const USkill* NewSkill) const
{
	return !IsActivate() || (Skill->GetPriority() < NewSkill->GetPriority());
}

void UCast::OnRelease()
{
	Skill = nullptr;
}

bool UCast::IsActivate_Implementation() const
{
	return Skill != nullptr;
}
