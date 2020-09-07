// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetworkObject.h"
#include "Data/AttackPart.h"
#include "SkillContext.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnAnimationEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHit, AActor*, Target);

UCLASS(BlueprintType)
class PROJECTR_API USkillContext final : public UNetworkObject
{
	GENERATED_BODY()

public:
	void Initialize(const TArray<class UPrimitiveComponent*>& InComponents);
	
	UFUNCTION(BlueprintCallable)
	void SetCollision(UPARAM(meta = (Bitmask, BitmaskEnum = EAttackPart)) int32 AttackPart);

	UFUNCTION(BlueprintCallable)
	void PlayAnimation(class UAnimMontage* Animation, const FOnAnimationEnded& OnAnimationEnded);

	UFUNCTION(BlueprintCallable)
	void StopAnimation(UAnimMontage* Animation);

private:
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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
	FOnHit OnHit;

private:
	UPROPERTY(Transient)
	TArray<UPrimitiveComponent*> Components;

	UPROPERTY(Transient)
	TMap<UAnimMontage*, FOnAnimationEnded> Callbacks;
};
