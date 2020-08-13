// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PRAnimInstance.generated.h"

UCLASS()
class PROJECTR_API UPRAnimInstance final : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPRAnimInstance();

private:
	void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = true))
	FVector Velocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = true))
	uint8 bIsLooking : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = true))
	uint8 bIsInAir : 1;
};
