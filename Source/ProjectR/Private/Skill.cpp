// Fill out your copyright notice in the Description page of Project Settings.

#include "Skill.h"
#include "Engine/World.h"
#include "Buff/Cast.h"
#include "Character/ProjectRCharacter.h"
#include "Character/ProjectRPlayerState.h"
#include "Data/SkillData.h"
#include "BuffLibrary.h"
#include "Weapon.h"

void USkill::BeginPlay(const FSkillData& SkillData)
{
	Weapon = Cast<UWeapon>(GetOuter());
	User = Cast<AProjectRCharacter>(Weapon->GetOuter());

	Priority = SkillData.Priority;
	CoolTime = SkillData.CoolTime;
	UseEnergy = SkillData.UseEnergy;

	ReceiveBeginPlay(SkillData.AdditionalData);
}

void USkill::Start()
{
	if (GetPriority() >= 0)
		UBuffLibrary::GetBuff<UCast>(User)->CastSkill(this);

	ReceiveStart();
}

void USkill::End()
{
	if (GetPriority() >= 0)
		UBuffLibrary::ReleaseBuff<UCast>(User);

	ReceiveEnd();
}

UWorld* USkill::GetWorld() const
{
	return User ? User->GetWorld() : nullptr;
}

bool USkill::IsNotCoolTime() const
{
	return FMath::IsNearlyEqual(NextUseTime, 0.0f) || NextUseTime <= GetWorld()->GetTimeSeconds();
}

void USkill::ApplyCooltime()
{
	NextUseTime = GetWorld()->GetTimeSeconds() + CoolTime;
}

bool USkill::IsEnoughEnergy() const
{
	auto* PlayerState = User->GetPlayerState<AProjectRPlayerState>();
	if (!PlayerState) return true;

	return PlayerState->GetEnergy() >= UseEnergy;
}

void USkill::ApplyEnergy()
{
	if (auto* PlayerState = User->GetPlayerState<AProjectRPlayerState>())
		PlayerState->HealEnergy(-UseEnergy);
}

bool USkill::CanUse_Implementation() const
{
	return UBuffLibrary::GetBuff<UCast>(User)->CanUseSkill(this);
}
