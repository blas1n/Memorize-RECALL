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

void ASkill::UseSkill()
{
	if (IsValid(User) && !User->IsCasting())
		OnSkill();
}

bool ASkill::CanUseSkill()
{
	bool bIsUsableEnergy = true;
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(User))
		bIsUsableEnergy = Player->GetEnergy() >= UseEnergy;
	
	return !IsCoolTime() && bIsUsableEnergy;
}

void ASkill::ApplyCooltime()
{
	NextUseTime = GetWorld()->GetTimeSeconds() + CoolTime;
}

void ASkill::ApplyEnergy()
{
	if (APlayerCharacter* Player = Cast<APlayerCharacter>(User))
		Player->HealEnergy(-UseEnergy);
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
