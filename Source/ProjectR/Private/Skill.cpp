// Fill out your copyright notice in the Description page of Project Settings.

#include "Skill.h"
#include "Engine/World.h"
#include "ProjectRCharacter.h"
#include "ProjectRPlayerState.h"
#include "Weapon.h"

void USkill::Initialize()
{
	Weapon = Cast<UWeapon>(GetOuter());
	User = Cast<AProjectRCharacter>(Weapon->GetOuter());
	OnInitialize();
}

void USkill::Use()
{
	if (CanUse()) OnUse();
}

UActorComponent* USkill::NewComponent(TSubclassOf<UActorComponent> Class)
{
	auto* Component = NewObject<UActorComponent>(User, Class);
	Component->RegisterComponent();
	return Component;
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
