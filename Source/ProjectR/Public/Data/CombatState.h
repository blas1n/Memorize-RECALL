// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CombatState.generated.h"

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	None, Attack, Dodge,
};
