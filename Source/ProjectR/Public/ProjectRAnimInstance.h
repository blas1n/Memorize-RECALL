// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ProjectRAnimInstance.generated.h"

UCLASS()
class PROJECTR_API UProjectRAnimInstance final : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UProjectRAnimInstance();

private:
	void NativeInitializeAnimation() override;
	void NativeUpdateAnimation(float DeltaSeconds) override;

	UFUNCTION()
	void OnEquipped(class AWeapon* Weapon);

	UFUNCTION()
	void OnWeaponAsyncLoaded();

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

	AWeapon* GetWeapon() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = true))
	class UBlendSpaceBase* LocomotionSpace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = true))
	class UAnimSequenceBase* JumpStart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimSequenceBase* JumpLoop;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Animation, meta = (AllowPrivateAccess = true))
	UAnimSequenceBase* JumpEnd;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pawn, meta = (AllowPrivateAccess = true))
	bool bIsInAir;
};
