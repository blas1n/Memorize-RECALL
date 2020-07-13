// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LogicData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FLogicData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<class UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PatrolRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectionIncrease;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectionDecrease;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float QuestDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LoseSightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FOV;
};
