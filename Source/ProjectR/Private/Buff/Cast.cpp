// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Cast.h"
#include "Skill.h"

void UCast::StopSkill()
{
	if (!IsActivate()) return;
	
	CurSkill->End();
	CurSkill = nullptr;
}

void UCast::SetCurSkill(USkill* Skill)
{
	if (!CanUseSkill(Skill)) return;

	StopSkill();
	CurSkill = Skill;
}

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
