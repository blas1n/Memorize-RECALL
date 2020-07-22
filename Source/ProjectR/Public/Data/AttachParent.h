// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttachParent.generated.h"

UENUM(BlueprintType)
enum class EAttachParent : uint8
{
	None, Mesh, RightWeapon, LeftWeapon,
};
