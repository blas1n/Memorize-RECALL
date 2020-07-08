// Fill out your copyright notice in the Description page of Project Settings.

#include "Skill.h"
#include "Engine/World.h"
#include "Character/ProjectRCharacter.h"
#include "Character/ProjectRPlayerState.h"
#include "Weapon.h"

void USkill::BeginPlay()
{
	Weapon = Cast<UWeapon>(GetOuter());
	User = Cast<AProjectRCharacter>(Weapon->GetOuter());
	ReceiveBeginPlay();
}

void USkill::Start()
{
	if (Priority >= 0)
	{
		auto* Cast = UBuffLibrary::GetBuff<UCast>(User);
		Cast->SetCurSkill(this);
		Cast->Apply();
	}

	Weapon->OnBeginSkill.Broadcast(this);
	ReceiveStart();
}

void USkill::End()
{
	if (Priority >= 0)
		UBuffLibrary::ReleaseBuff<UCast>(User);

	Weapon->OnEndSkill.Broadcast(this);
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
	return User->GetPlayerState<AProjectRPlayerState>()->GetEnergy() >= UseEnergy;
}

void USkill::ApplyEnergy()
{
	User->GetPlayerState<AProjectRPlayerState>()->HealEnergy(-UseEnergy);
}
