// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/SingleAttackData.h"
#include "LeapData.generated.h"

UCLASS(BlueprintType)
class ULeapData : public USingleAttackData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHeight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LeapDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxHeightRatio;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LeapTimeRatio;
};
