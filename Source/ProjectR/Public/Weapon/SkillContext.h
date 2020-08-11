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
	UFUNCTION(BlueprintCallable)
	void SetWeaponMesh(class UStaticMesh* RightMesh, const FTransform&
		RightTransform, UStaticMesh* LeftMesh, const FTransform& LeftTransform);

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(bool bEnableRight, bool bEnableLeft);

	UFUNCTION(BlueprintCallable)
	void PlayAnimation(class UAnimMontage* Animation);

	UFUNCTION(BlueprintCallable)
	void StopAnimation();

private:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayAnimation(UAnimMontage* Animation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastStopAnimation(UAnimMontage* Animation);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetWeaponMesh(UStaticMesh* RightMesh, const FTransform&
		RightTransform, UStaticMesh* LeftMesh, const FTransform& LeftTransform);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnWeaponOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const struct FHitResult& SweepResult);

	void MulticastPlayAnimation_Implementation(UAnimMontage* Animation);
	void MulticastStopAnimation_Implementation(UAnimMontage* Animation);

	void MulticastSetWeaponMesh_Implementation(UStaticMesh* RightMesh,
		const FTransform& RightTransform, UStaticMesh* LeftMesh, const FTransform& LeftTransform);
public:
	UPROPERTY(BlueprintAssignable)
	FOnAnimationEnded OnAnimationEnded;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponOverlapped OnWeaponOverlapped;

private:
	friend class UWeaponComponent;

	void Initialize(class UStaticMeshComponent* InRightWeapon, UStaticMeshComponent* InLeftWeapon);

	UPROPERTY(Transient)
	UStaticMeshComponent* RightWeapon;

	UPROPERTY(Transient)
	UStaticMeshComponent* LeftWeapon;

	UPROPERTY(Transient)
	UAnimMontage* PlayingMontage;
};
