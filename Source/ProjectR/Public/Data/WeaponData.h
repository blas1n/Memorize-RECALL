// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SkillData.h"
#include "WeaponData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Key;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSkillData> SkillsData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UAnimInstance> UpperAnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<class UAnimMontage> EquipAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<class UStaticMesh> RightMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RightTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<UStaticMesh> LeftMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform LeftTransform;
};
