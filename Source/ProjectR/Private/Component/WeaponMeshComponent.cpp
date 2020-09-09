// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/WeaponMeshComponent.h"
#include "Component/WeaponComponent.h"

UWeaponMeshComponent::UWeaponMeshComponent()
	: Super()
{
	SetCollisionProfileName(TEXT("Weapon"));
}

void UWeaponMeshComponent::SetWeapon(USkeletalMesh* Mesh,
	TSubclassOf<UAnimInstance> Anim, const FTransform& Transform)
{
	if (Mesh == SkeletalMesh) return;

	if (!HasBegunPlay())
	{
		SetMesh(Mesh, Anim, Transform.GetLocation(), Transform.GetRotation());
		SetRelativeScale3D(Transform.GetScale3D());
		return;
	}

	bNowDecrease = SkeletalMesh != nullptr;
	bNeedFast = bNowDecrease && (Mesh != nullptr);

	if (bNowDecrease)
	{
		OriginalMesh = Mesh;
		OriginalAnim = Anim;
		OriginalTransform = Transform;
		BeforeScale = GetRelativeScale3D();
	}
	else
	{
		SetMesh(Mesh, Anim, Transform.GetLocation(), Transform.GetRotation());
		OriginalTransform.SetScale3D(Transform.GetScale3D());
		SetRelativeScale3D(FVector::ZeroVector);
	}

	SetMaterial(0, SwapMaterial);
	SwapRatio = 1.0f;
}

void UWeaponMeshComponent::Detach()
{
	SetCollisionProfileName(TEXT("Ragdoll"));
	SetSimulatePhysics(true);

	const auto Rules = FDetachmentTransformRules::KeepWorldTransform;
	DetachFromComponent(Rules);
}

void UWeaponMeshComponent::SetTrail(bool bOnTrail)
{
	bEnableTrail = bOnTrail;
}

void UWeaponMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SwapRatio <= 0.0f) return;

	auto* WeaponComp = Cast<UWeaponComponent>(GetOwner()
		->GetComponentByClass(UWeaponComponent::StaticClass()));

	check(WeaponComp);

	float SwapDuration = WeaponComp->GetWeaponSwapDuration();
	if (SwapDuration <= 0.0f) return;
	
	if (bNeedFast) SwapDuration *= 0.5f;
	SwapRatio -= GetWorld()->GetDeltaSeconds() * (1.0f / SwapDuration);
	SwapRatio = FMath::Max(SwapRatio, 0.0f);

	if (bNowDecrease)
	{
		if (SetScaleIfNeed(FVector::ZeroVector, BeforeScale))
			return;

		SetMesh(OriginalMesh, OriginalAnim, OriginalTransform.GetLocation(), OriginalTransform.GetRotation());
		SetRelativeScale3D(FVector::ZeroVector);
		
		if (OriginalMesh)
		{
			bNowDecrease = false;
			SwapRatio = 1.0f;
		}
		else EmptyOverrideMaterials();
	}
	else if (!SetScaleIfNeed(OriginalTransform.GetScale3D(), FVector::ZeroVector))
	{
		SetRelativeScale3D(OriginalTransform.GetScale3D());
		EmptyOverrideMaterials();
	}
}

void UWeaponMeshComponent::SetMesh(USkeletalMesh* Mesh,
	TSubclassOf<UAnimInstance> Anim, const FVector& Loc, const FQuat& Rot)
{
	SetAnimClass(nullptr); // It need to validate skeletal mesh
	SetSkeletalMesh(Mesh);
	SetAnimClass(Anim);
	SetRelativeLocationAndRotation(Loc, Rot);
}

bool UWeaponMeshComponent::SetScaleIfNeed(const FVector& A, const FVector& B)
{
	if (SwapRatio > 0.0f)
	{
		SetRelativeScale3D(FMath::Lerp(A, B, SwapRatio));
		return true;
	}

	return false;
}
