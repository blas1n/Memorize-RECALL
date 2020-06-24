// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ProjectRAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EWalkState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Forward UMETA(DisplayName = "Forward"),
	Backward UMETA(DisplayName = "Backward"),
};

UCLASS()
class PROJECTR_API UProjectRAnimInstance final : public UAnimInstance
{
	GENERATED_BODY()

public:
	UProjectRAnimInstance();

private:
	void NativeBeginPlay() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void OnBeginMontage(UAnimMontage* Montage);

	UFUNCTION()
	void OnEndMontage(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void AnimNotify_BeginSkill();

	UFUNCTION()
	void AnimNotify_EndSkill();

	UFUNCTION()
	void AnimNotify_BeginAttack();

	UFUNCTION()
	void AnimNotify_EndAttack();

	UFUNCTION()
	void AnimNotify_Shoot();

	UFUNCTION()
	void AnimNotify_Execute();

	UFUNCTION()
	void AnimNotify_EnableMove();

	UFUNCTION()
	void AnimNotify_DisableMove();

	class UWeapon* GetWeapon() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = true))
	class AProjectRCharacter* User;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = true))
	EWalkState WalkState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = true))
	uint8 bIsRunning : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = true))
	uint8 bIsCrouched : 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = true))
	uint8 bIsInAir : 1;
};
