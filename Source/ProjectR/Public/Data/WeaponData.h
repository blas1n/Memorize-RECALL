// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 ComboHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ComboDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class USkill> WeakAttackClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USkill> StrongAttackClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USkill> ParryingClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<uint8, TSubclassOf<USkill>> Skills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<class UStaticMesh> RightMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RightTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<UStaticMesh> LeftMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform LeftTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UAnimInstance> UpperAnimInstance;
};
