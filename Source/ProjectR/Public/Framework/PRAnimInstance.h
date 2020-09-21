// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Data/AnimData.h"
#include "PRAnimInstance.generated.h"

UCLASS()
class PROJECTR_API UPRAnimInstance final : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPRAnimInstance();

	FORCEINLINE void SetAnimData(const FAnimData& InAnimData) noexcept { AnimData = InAnimData; }

private:
	void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FAnimData AnimData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector2D Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float BlendRatio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bIsLocking : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bIsInAir : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bIsDeath : 1;
};
