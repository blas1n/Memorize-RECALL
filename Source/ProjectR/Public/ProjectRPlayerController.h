// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjectRPlayerController.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTR_API AProjectRPlayerController final : public APlayerController
{
	GENERATED_BODY()

private:
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void SetupInputComponent() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void InputYaw(float Value);
	void InputPitch(float Value);
	
	void PressDodge();
	void ReleaseDodge();

	void Run();
	void Walk();

	void SwapWeapon(uint8 Index);
	void SwapWeapon(float Value);

	void UseSkill(uint8 Index);

	void LockOn();
	void LockOff();
	bool CheckLockOn(const AActor* Enemy, float& OutAngle, float& OutDistance) const;

	void SetUserTransformByInput();
	void CheckLockTarget();
	void Turn(float DeltaSeconds);

	FVector GetDirectionVectorByActor(EAxis::Type Axis) const noexcept;
	FVector GetDirectionVectorByController(EAxis::Type Axis) const noexcept;

private:
	UPROPERTY()
	class AProjectRCharacter* User;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float TurnRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float DodgeDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float LockOnDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float LoseLockOnDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float LockOnAngle;

	FRotator TurnRotation;
	FVector MoveInput;
	uint8 bIsTurning : 1;
};
