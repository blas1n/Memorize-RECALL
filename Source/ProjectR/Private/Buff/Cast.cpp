// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Cast.h"
#include "Skill.h"

void UCast::StopCast()
{
	if (!IsActivate())
		return;
	
	Skill->End();
	Release();
}

void UCast::SetCastSkill(USkill* NewSkill)
{
	StopCast();
	Skill = NewSkill;
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
