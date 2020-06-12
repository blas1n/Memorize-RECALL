// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMetaData.h"
#include "AnimCastData.generated.h"

UCLASS()
class PROJECTR_API UAnimCastData : public UAnimMetaData
{
	GENERATED_BODY()
	
public:
	UAnimCastData() : Super(), bIsCasting(true), bCanMoving(false) {}

	FORCEINLINE bool IsCasting() const noexcept { return bIsCasting; }
	FORCEINLINE bool CanMoving() const noexcept { return bCanMoving; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bIsCasting : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bCanMoving : 1;
};
