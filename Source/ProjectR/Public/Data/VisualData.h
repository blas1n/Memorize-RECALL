// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "VisualData.generated.h"

USTRUCT(Atomic)
struct FVisualData
{
	GENERATED_BODY()

	UPROPERTY()
	class UStaticMesh* RightMesh;

	UPROPERTY()
	FTransform RightTransform;

	UPROPERTY()
	UStaticMesh* LeftMesh;

	UPROPERTY()
	FTransform LeftTransform;

	UPROPERTY()
	TSubclassOf<class UAnimInstance> UpperAnimInstance;
};