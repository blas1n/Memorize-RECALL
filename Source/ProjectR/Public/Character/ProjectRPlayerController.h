// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "Interface/Directionable.h"
#include "ProjectRPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);

UCLASS(Abstract, Blueprintable)
class PROJECTR_API AProjectRPlayerController final : public APlayerController, public IGenericTeamAgentInterface, public IDirectionable
{
	GENERATED_BODY()

private:
	void OnPossess(APawn* InPawn) override;
	void OnUnPossess() override;

	void Tick(float DeltaSeconds) override;
	void SetupInputComponent() override;

	FGenericTeamId GetGenericTeamId() const { return FGenericTeamId{ 0 }; }
	FVector2D GetDirection_Implementaion() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void InputYaw(float Value);
	void InputPitch(float Value);
	
	void PressDodge();
	void ReleaseDodge();
	void Dodge();

	void Run();
	void Walk();

	void SwapWeapon(uint8 Index);
	void SwapWeapon(float Value);

	void UseSkill(uint8 Index);

	void Interact();

	void LockOn();
	void LockOff();

	TArray<FOverlapResult> GetLockableEnemys() const;
	bool CheckLockOn(const FOverlapResult& Enemy) const;

	void CheckLockTarget();
	void Turn(float DeltaSeconds);

	FVector GetDirectionVector(EAxis::Type Axis) const;

	UFUNCTION()
	void OnDeath();

public:
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnInteract;

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

	FVector2D Direction;

	FRotator TurnRotation;
	uint8 bIsTurning : 1;
};
