// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FCharacterData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TAssetPtr<class USkeletalMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UPRAnimInstance> AnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CapsuleHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CapsuleRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MeshZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MeshYaw;
};
