// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AttackPart.generated.h"

UENUM(BlueprintType, meta = (Bitflags))
enum EAttackPart // Can add other part. (Ex. Head)
{
	None = 0x00 UMETA(Hidden),
	RightWeapon = 0x01 UMETA(DisplayName = "RightWeapon"),
	LeftWeapon = 0x02 UMETA(DisplayName = "LeftWeapon"),
	RightFoot = 0x04 UMETA(DisplayName = "RightFoot"),
	LeftFoot = 0x08 UMETA(DisplayName = "LeftFoot"),
};
