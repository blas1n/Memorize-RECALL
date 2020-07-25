// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MobData.generated.h"

UCLASS(Blueprintable)
class UMobData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UCurveFloat* DetectionCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ImmediateDetectionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DecreaseDetectionValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float QuestDuration;
};
