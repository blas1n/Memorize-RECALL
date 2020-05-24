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
	IsPlayer = false;
}

void ASkill::UseSkill()
{
	if (IsCoolTime()) return;
	if (IsPlayer)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(User);
		if (Player->GetEnergy() < UseEnergy) return;

		Player->HealEnergy(-UseEnergy);
	}

	NextUseTime = GetWorld()->GetTimeSeconds() + CoolTime;
	OnActive();
}

void ASkill::BeginPlay()
{
	Super::BeginPlay();
	
	User = Cast<AProjectRCharacter>(GetInstigator());
	IsPlayer = Cast<APlayerCharacter>(User) != nullptr;
}

bool ASkill::IsCoolTime() const
{
	return (!FMath::IsNearlyEqual(NextUseTime, 0.0f) && NextUseTime > GetWorld()->GetTimeSeconds());
}
