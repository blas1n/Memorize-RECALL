// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetworkObject.h"
#include "SkillContext.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimationEnded);

UCLASS(BlueprintType)
class PROJECTR_API USkillContext final : public UNetworkObject
{
	GENERATED_BODY()

public:
	void Initialize(class UStaticMeshComponent* InRightWeapon, UStaticMeshComponent* InLeftWeapon);
	
	UFUNCTION(BlueprintCallable)
	void PlayAnimation(class UAnimMontage* Animation);

	UFUNCTION(BlueprintCallable)
	void StopAnimation(UAnimMontage* Animation);

private:
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAnimation(UAnimMontage* Animation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopAnimation(UAnimMontage* Animation);

	void MulticastPlayAnimation_Implementation(UAnimMontage* Animation);
	void MulticastStopAnimation_Implementation(UAnimMontage* Animation);

public:
	UPROPERTY(BlueprintAssignable)
	FOnAnimationEnded OnAnimationEnded;

private:
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* RightWeapon;
	
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* LeftWeapon;
};
