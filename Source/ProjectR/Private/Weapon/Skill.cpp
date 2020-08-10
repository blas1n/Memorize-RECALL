// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Skill.h"
#include "GameFramework/Actor.h"
#include "Component/WeaponComponent.h"
#include "Interface/ComponentOwner.h"

void USkill::Initialize(UWeaponContext* InContext, UObject* Data)
{
	User = GetTypedOuter<AActor>();
	Context = InContext;

	ReceiveInitialize(Data);
}

void USkill::Begin()
{
	if (bIsExecute)
		return;

	bIsExecute = true;
	ReceiveBegin();
}

void USkill::End()
{
	if (!bIsExecute)
		return;

	bIsExecute = false;
	ReceiveEnd();

	IComponentOwner::Execute_GetWeaponComponent(User)->OnEndSkill();
}

UWorld* USkill::GetWorld() const
{
	return User ? User->GetWorld() : nullptr;
}
