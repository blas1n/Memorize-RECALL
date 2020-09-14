// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "WeaponMeshComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTR_API UWeaponMeshComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()
	
public:
	UWeaponMeshComponent();

	void SetWeapon(USkeletalMesh* Mesh,
		TSubclassOf<UAnimInstance> Anim, const FTransform& Transform);

	void Detach();

private:
	void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	void SetMesh(USkeletalMesh* Mesh, TSubclassOf<UAnimInstance> Anim,
		const FVector& Loc, const FQuat& Rot);

	bool SetScaleIfNeed(const FVector& A, const FVector& B);

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	UMaterialInterface* SwapMaterial;

	UPROPERTY(Transient)
	USkeletalMesh* OriginalMesh;

	TSubclassOf<UAnimInstance> OriginalAnim;
	FTransform OriginalTransform;
	FVector BeforeScale;

	float SwapRatio;

	uint8 bNeedFast : 1;
	uint8 bNowDecrease : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	uint8 bEnableTrail : 1;
};
