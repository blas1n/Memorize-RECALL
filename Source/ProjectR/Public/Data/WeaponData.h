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
	TSubclassOf<class USkill> AttackClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<USkill> DodgingClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<uint8, TSubclassOf<USkill>> Skills;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<class USkeletalMesh> RightMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RightTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<class UAnimationAsset> RightTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<USkeletalMesh> LeftMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform LeftTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<UAnimationAsset> LeftTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<class UBlendSpace1D> NotLockAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<class UBlendSpace> LockAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<class UAnimSequence> AirAnim;
};
