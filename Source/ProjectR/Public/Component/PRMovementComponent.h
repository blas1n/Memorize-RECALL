// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "PRMovementComponent.generated.h"

UCLASS()
class PROJECTR_API UPRMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

private:
	void AddInputVector(FVector WorldVector, bool bForce = false) override;

	FVector ConsumeInputVector() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetLastInputVector(FVector InLastInputVector);

	FORCEINLINE void ServerSetLastInputVector_Implementation
		(FVector InLastInputVector) noexcept { LastInputVector = InLastInputVector;	}

	FORCEINLINE bool ServerSetLastInputVector_Validate
		(FVector InLastInputVector) const noexcept { return true; }

private:
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector LastInputVector;

	FVector InputVector;
};
