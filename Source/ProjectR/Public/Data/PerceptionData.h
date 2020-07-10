// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PerceptionData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FPerceptionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LoseSightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FOV;
};
