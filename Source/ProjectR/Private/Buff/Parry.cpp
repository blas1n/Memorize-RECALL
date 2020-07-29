// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Parry.h"
#include "Interface/Parryable.h"

bool UParry::ParryIfCan(int32 Damage, AController* Instigator, AProjectRCharacter* Causer)
{
	const auto bCanParry = IsActivate() && ParryObject &&
		IParryable::Execute_IsParryable(ParryObject, Damage, Instigator, Causer);

	if (bCanParry) IParryable::Execute_Parry(ParryObject, Damage, Instigator, Causer);
	return bCanParry;
}

void UParry::SetParryObject(UObject* Object)
{
	if (Object->GetClass()->ImplementsInterface(UParryable::StaticClass()))
		ParryObject = Object;
}

void UParry::OnApply()
{
	bIsParry = true;
}

void UParry::OnRelease()
{
	bIsParry = false;
}

bool UParry::IsActivate_Implementation() const
{
	return bIsParry;
}
