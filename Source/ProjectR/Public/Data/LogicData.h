// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LogicData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FLogicData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PatrolRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float QuestRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LockRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectionIncrease;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DetectionDecrease;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LoseSightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FOV;
};
