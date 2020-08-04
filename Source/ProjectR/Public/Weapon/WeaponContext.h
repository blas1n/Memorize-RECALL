// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeaponContext.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponOverlapped, AActor*, Target);

UCLASS(BlueprintType)
class PROJECTR_API UWeaponContext final : public UObject
{
	GENERATED_BODY()

public:
	void SetWeaponMesh(class UStaticMesh* RightMesh, const FTransform&
		RightTransform, UStaticMesh* LeftMesh, const FTransform& LeftTransform);

	void SetWeaponCollision(bool bEnableRight, bool bEnableLeft);

private:
	UFUNCTION()
	void OnWeaponOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const struct FHitResult& SweepResult);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetWeaponMesh(UStaticMesh* RightMesh, const FTransform&
		RightTransform, UStaticMesh* LeftMesh, const FTransform& LeftTransform);

	void MulticastSetWeaponMesh_Implementation(UStaticMesh* RightMesh,
		const FTransform& RightTransform, UStaticMesh* LeftMesh, const FTransform& LeftTransform);

public:
	UPROPERTY(BlueprintAssignable)
	FOnWeaponOverlapped OnWeaponOverlapped;

private:
	friend class UWeaponComponent;

	void Initialize(class UStaticMeshComponent* InRightWeapon, class UStaticMeshComponent* InLeftWeapon);

	UPROPERTY(Transient)
	UStaticMeshComponent* RightWeapon;

	UPROPERTY(Transient)
	UStaticMeshComponent* LeftWeapon;
};
