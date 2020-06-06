// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerSkill.h"
#include "PlayerCharacter.h"

APlayerSkill::APlayerSkill()
	: Super()
{
	UseEnergy = 0;
}

bool APlayerSkill::IsEnoughEnergy() const
{
	return Cast<APlayerCharacter>(GetUser())->GetEnergy() >= UseEnergy;
}

void APlayerSkill::ApplyEnergy()
{
	Cast<APlayerCharacter>(GetUser())->HealEnergy(-UseEnergy);
}