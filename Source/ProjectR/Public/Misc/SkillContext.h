// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetworkObject.h"
#include "SkillContext.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnimationEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponOverlapped, AActor*, Target);

UCLASS(BlueprintType)
class PROJECTR_API USkillContext final : public UNetworkObject
{
	GENERATED_BODY()

public:
	void Initialize(class UStaticMeshComponent* InRightMeshComp, UStaticMeshComponent* InLeftMeshComp);

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(bool bEnableRight, bool bEnableLeft);

	UFUNCTION(BlueprintCallable)
	void PlayAnimation(class UAnimMontage* Animation);

	UFUNCTION(BlueprintCallable)
	void StopAnimation();

private:
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnWeaponOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const struct FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAnimation(UAnimMontage* Animation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopAnimation(UAnimMontage* Animation);

	void MulticastPlayAnimation_Implementation(UAnimMontage* Animation);
	void MulticastStopAnimation_Implementation(UAnimMontage* Animation);

public:
	UPROPERTY(BlueprintAssignable)
	FOnAnimationEnded OnAnimationEnded;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponOverlapped OnWeaponOverlapped;

private:
	UPROPERTY(Transient)
	UStaticMeshComponent* RightWeaponComp;

	UPROPERTY(Transient)
	UStaticMeshComponent* LeftWeaponComp;

	UPROPERTY(Transient)
	UAnimMontage* PlayingMontage;
};
