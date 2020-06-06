// Fill out your copyright notice in the Description page of Project Settings.

#include "Skill.h"
#include "Engine/World.h"
#include "ProjectRCharacter.h"

ASkill::ASkill()
	: Super()
{
 	PrimaryActorTick.bCanEverTick = false;

	User = nullptr;
	Weapon = nullptr;
	CoolTime = 0.0f;
	NextUseTime = 0.0f;
}

bool ASkill::UseSkill()
{
	if (User->IsCasting() || !CanUseSkill())
		return false;

	OnSkill();
	return true;
}

void ASkill::BeginPlay()
{
	User = Cast<AProjectRCharacter>(GetInstigator());
	Super::BeginPlay();
}

bool ASkill::IsNotCoolTime() const
{
	return FMath::IsNearlyEqual(NextUseTime, 0.0f) || NextUseTime <= GetWorld()->GetTimeSeconds();
}

void ASkill::ApplyCooltime()
{
	NextUseTime = GetWorld()->GetTimeSeconds() + CoolTime;
}