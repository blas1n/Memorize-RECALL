// Fill out your copyright notice in the Description page of Project Settings.

#include "Skill.h"
#include "Engine/World.h"
#include "ProjectRCharacter.h"
#include "ProjectRPlayerState.h"
#include "Weapon.h"

void USkill::Use()
{
	if (CanUse()) OnUse();
}

bool USkill::IsEnoughEnergy() const
{
	return User->GetPlayerState<AProjectRPlayerState>()->GetEnergy() >= UseEnergy;
}

void USkill::ApplyEnergy()
{
	User->GetPlayerState<AProjectRPlayerState>()->HealEnergy(-UseEnergy);
}

bool USkill::IsNotCoolTime() const
{
	return FMath::IsNearlyEqual(NextUseTime, 0.0f) || NextUseTime <= GetWorld()->GetTimeSeconds();
}

void USkill::ApplyCooltime()
{
	NextUseTime = GetWorld()->GetTimeSeconds() + CoolTime;
}

void USkill::Initialize_Implementation()
{
	Weapon = Cast<UWeapon>(GetOuter());
	User = Cast<AProjectRCharacter>(Weapon->GetOuter());
}
