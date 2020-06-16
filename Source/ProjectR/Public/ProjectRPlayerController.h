// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ProjectRPlayerController.generated.h"

DECLARE_DELEGATE(FOnKeyHolderUsed);

struct FKeyHolder
{
	FKeyHolder(FTimerHandle InTimer, FOnKeyHolderUsed InOnShort, FOnKeyHolderUsed InOnLong, float InDuration)
		: Timer(MoveTemp(InTimer)), OnShort(MoveTemp(InOnShort)), OnLong(MoveTemp(InOnLong)), Duration(MoveTemp(InDuration)) {}

	FTimerHandle Timer;
	FOnKeyHolderUsed OnShort;
	FOnKeyHolderUsed OnLong;
	float Duration;
};

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
	void Crouch();

	void SwapWeapon(uint8 Index);
	void SwapWeapon(float Value);

	void UseSkill(uint8 Index);

	void LockOn();
	bool CheckLockOn(const AActor* Enemy, float& OutAngle, float& OutDistance) const;

	void RegisterKeyHolder(const FName& Name, FOnKeyHolderUsed OnShort, FOnKeyHolderUsed OnLong, float Duration);
	void PressKeyHolder(const FName& Name);
	void ReleaseKeyHolder(const FName& Name);

	FVector GetDirectionVector(EAxis::Type Axis) const noexcept;

private:
	UPROPERTY()
	class AProjectRCharacter* User;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float DodgeDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float LockOnDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float LoseLockOnDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float LockOnAngle;

	TMap<FName, FKeyHolder> KeyHolders;
};
