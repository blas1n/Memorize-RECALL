// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "PRPlayerController.generated.h"

UCLASS(BlueprintType)
class PROJECTR_API APRPlayerController final : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	APRPlayerController();

	UFUNCTION(BlueprintCallable)
	void RegisterInteractor(UObject* InInteractor);
	
	UFUNCTION(BlueprintCallable)
	void UnregisterInteractor();

	FGenericTeamId GetGenericTeamId() const override;
	void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnSetInteractor(UObject* NewInteractor);

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

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UTargetComponent* Targeter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UAIPerceptionComponent* Perception;

	UPROPERTY(Transient)
	UObject* Interactor;
};
