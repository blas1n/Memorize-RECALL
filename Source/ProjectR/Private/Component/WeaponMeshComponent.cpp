// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/WeaponMeshComponent.h"

UWeaponMeshComponent::UWeaponMeshComponent()
	: Super()
{
	SetCollisionProfileName(TEXT("Weapon"));
}

void UWeaponMeshComponent::SetWeapon(USkeletalMesh* Mesh,
	TSubclassOf<UAnimInstance> Anim, const FTransform& Transform)
{
	SetSkeletalMesh(Mesh);
	SetAnimClass(Anim);
	SetRelativeTransform(Transform);
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
