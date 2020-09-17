// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SingleAttackData.generated.h"

UCLASS(BlueprintType)
class USingleAttackData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Bitmask, BitmaskEnum = "EAttackPart"))
	int32 AttackPart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UAnimMontage* Animation;
};
