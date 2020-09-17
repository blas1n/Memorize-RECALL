// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "PRPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteract);

UCLASS(BlueprintType)
class PROJECTR_API APRPlayerController final : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	APRPlayerController();
	
	FGenericTeamId GetGenericTeamId() const override;
	void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;

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

	void AttackWeak();
	void AttackStrong();

	void SwapWeapon(uint8 Index);
	void SwapWeapon(float Value);

	void Lock();
	void Unlock();
	void Relock();

	void Interact();

	FVector GetDirectionVector(EAxis::Type Axis) const;

public:
	UPROPERTY(BlueprintAssignable)
	FOnInteract OnInteract;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UTargetComponent* Targeter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UAIPerceptionComponent* Perception;
};
