// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/AnimData.h"
#include "VisualData.generated.h"

USTRUCT()
struct FVisualData
{
	GENERATED_BODY()

	UPROPERTY()
	class USkeletalMesh* RightMesh;

	UPROPERTY()
	FTransform RightTransform;

	UPROPERTY()
	USkeletalMesh* LeftMesh;

	UPROPERTY()
	FTransform LeftTransform;

	UPROPERTY()
	FAnimData AnimData;
};