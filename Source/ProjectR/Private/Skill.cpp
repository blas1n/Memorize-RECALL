// Fill out your copyright notice in the Description page of Project Settings.

#include "Skill.h"
#include "Engine/World.h"
#include "PlayerCharacter.h"

ASkill::ASkill()
{
 	PrimaryActorTick.bCanEverTick = false;

	User = nullptr;
	Weapon = nullptr;
	UseEnergy = 0;
	CoolTime = 0.0f;
	NextUseTime = 0.0f;
}

bool ASkill::CheckAndApplyLimit()
{
	if (IsCoolTime()) return false;
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(User))
	{
		if (Player->GetEnergy() < UseEnergy) return false;
		Player->HealEnergy(-UseEnergy);
	}

	NextUseTime = GetWorld()->GetTimeSeconds() + CoolTime;
	return true;
}

void ASkill::BeginPlay()
{
	User = Cast<AProjectRCharacter>(GetInstigator());
	Super::BeginPlay();
}

bool ASkill::IsCoolTime() const
{
	return (!FMath::IsNearlyEqual(NextUseTime, 0.0f) && NextUseTime > GetWorld()->GetTimeSeconds());
}
