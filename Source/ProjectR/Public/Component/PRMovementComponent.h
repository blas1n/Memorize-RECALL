// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "Data/MoveState.h"
#include "PRMovementComponent.generated.h"

UCLASS()
class PROJECTR_API UPRMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPRMovementComponent();

	UFUNCTION(BlueprintSetter)
	void SetRunSpeed(float InRunSpeed);

	UFUNCTION(BlueprintSetter)
	void SetWalkSpeed(float InWalkSpeed);

	UFUNCTION(BlueprintSetter)
	void SetLockSpeed(float InLockSpeed);

	UFUNCTION(BlueprintSetter)
	void SetMoveState(EMoveState NewMoveState);

	void ApplyLock(bool bIsLock);

	FORCEINLINE float GetRunSpeed() const noexcept { return RunSpeed; }
	FORCEINLINE float GetWalkSpeed() const noexcept { return WalkSpeed; }
	FORCEINLINE float GetLockSpeed() const noexcept { return LockSpeed; }
	FORCEINLINE EMoveState GetMoveState() const noexcept { return MoveState; }

private:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void AddInputVector(FVector WorldVector, bool bForce = false) override;

	FVector ConsumeInputVector() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetLastInputVector(FVector InLastInputVector);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetRunSpeed(float InRunSpeed);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetWalkSpeed(float InWalkSpeed);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetLockSpeed(float InLockSpeed);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetMoveState(EMoveState NewMoveState);

	FORCEINLINE void ServerSetLastInputVector_Implementation
		(FVector InLastInputVector) noexcept { LastInputVector = InLastInputVector;	}

	FORCEINLINE bool ServerSetLastInputVector_Validate
		(FVector InLastInputVector) const noexcept { return true; }

	void ServerSetRunSpeed_Implementation(float InRunSpeed);
	FORCEINLINE bool ServerSetRunSpeed_Validate
		(float InRunSpeed) const noexcept { return true; }

	void ServerSetWalkSpeed_Implementation(float InWalkSpeed);
	FORCEINLINE bool ServerSetWalkSpeed_Validate
		(float InWalkSpeed) const noexcept { return true; }

	void ServerSetLockSpeed_Implementation(float InLockSpeed);
	FORCEINLINE bool ServerSetLockSpeed_Validate
		(float InLockSpeed) const noexcept { return true; }

	void ServerSetMoveState_Implementation(EMoveState NewMoveState);
	FORCEINLINE bool ServerSetMoveState_Validate
		(EMoveState NewMoveState) const noexcept { return true; }

	UFUNCTION()
	void OnRep_RunSpeed();

	UFUNCTION()
	void OnRep_WalkSpeed();

	UFUNCTION()
	void OnRep_LockSpeed();

	UFUNCTION()
	void OnRep_MoveState();

	void SetMovement();

private:
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FVector LastInputVector;

	FVector InputVector;

	UPROPERTY(ReplicatedUsing = OnRep_RunSpeed, EditAnywhere,
		BlueprintSetter = SetRunSpeed, Category = Speed, meta = (AllowPrivateAccess = true))
	float RunSpeed;

	UPROPERTY(ReplicatedUsing = OnRep_WalkSpeed, EditAnywhere,
		BlueprintSetter = SetWalkSpeed, Category = Speed, meta = (AllowPrivateAccess = true))
	float WalkSpeed;

	UPROPERTY(ReplicatedUsing = OnRep_LockSpeed, EditAnywhere,
		BlueprintSetter = SetLockSpeed, Category = Speed, meta = (AllowPrivateAccess = true))
	float LockSpeed;

	UPROPERTY(ReplicatedUsing = OnRep_MoveState, Transient,
		BlueprintSetter = SetMoveState, meta = (AllowPrivateAccess = true))
	EMoveState MoveState;

	UPROPERTY(Transient)
	uint8 bIsLocked : 1;
};
