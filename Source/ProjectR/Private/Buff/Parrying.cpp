// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Parrying.h"
#include "Parryable.h"

bool UParrying::ParryIfCan(int32 Damage, AController* Instigator, AProjectRCharacter* Causer)
{
	const auto bCanParry = IsActivate() && ParryingObject &&
		IParryable::Execute_IsParryable(ParryingObject, Damage, Instigator, Causer);

	if (bCanParry) IParryable::Execute_Parry(ParryingObject, Damage, Instigator, Causer);
	return bCanParry;
}

void UParrying::SetParryingObject(UObject* Object)
{
	if (Object->GetClass()->ImplementsInterface(UParryable::StaticClass()))
		ParryingObject = Object;
}

void UParrying::OnApply()
{
	bIsParrying = true;
}

void UParrying::OnRelease()
{
	bIsParrying = false;
}

bool UParrying::IsActivate_Implementation() const
{
	return bIsParrying;
}
