// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AttackPart.generated.h"

UENUM(BlueprintType, meta = (Bitflags))
enum EAttackPart // Can add other part. (Ex. Head)
{
	RightWeapon,
	LeftWeapon,
	RightFoot,
	LeftFoot,
};
