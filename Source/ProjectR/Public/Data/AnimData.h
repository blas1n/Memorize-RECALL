// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AnimData.generated.h"

USTRUCT(BlueprintType)
struct FAnimData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	class UBlendSpace1D* NotLock;

	UPROPERTY(BlueprintReadWrite)
	class UBlendSpace* Lock;

	UPROPERTY(BlueprintReadWrite)
	class UAnimSequence* Air;
};