// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "ProjectRDamageType.generated.h"

UCLASS(const, Blueprintable, BlueprintType)
class PROJECTR_API UProjectRDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
	FORCEINLINE bool NeedReaction() const noexcept { return bNeedReaction; }
	FORCEINLINE bool IsWeaponAttack() const noexcept { return bIsWeaponAttack; }
	FORCEINLINE bool CanHealByDamage() const noexcept { return bCanHealByDamage; }

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bNeedReaction : 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bIsWeaponAttack : 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bCanHealByDamage : 1;
};
