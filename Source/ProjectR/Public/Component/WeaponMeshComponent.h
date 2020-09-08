// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponMeshComponent.generated.h"

UCLASS()
class PROJECTR_API UWeaponMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	UWeaponMeshComponent();

	void SetWeapon(USkeletalMesh* Mesh,
		TSubclassOf<UAnimInstance> Anim, const FTransform& Transform);

	void Detach();

	UFUNCTION(BlueprintCallable)
	void SetTrail(bool bOnTrail);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bEnableTrail : 1;
};
