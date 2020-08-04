// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/WeaponContext.h"
#include "Components/StaticMeshComponent.h"

void UWeaponContext::SetWeaponMesh(UStaticMesh* RightMesh, const FTransform&
	RightTransform, UStaticMesh* LeftMesh, const FTransform& LeftTransform)
{
	MulticastSetWeaponMesh(RightMesh, RightTransform, LeftMesh, LeftTransform);
}

void UWeaponContext::SetWeaponCollision(bool bEnableRight, bool bEnableLeft)
{
	check(GetTypedOuter<AActor>()->HasAuthority());

	RightWeapon->SetCollisionEnabled(bEnableRight ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	LeftWeapon->SetCollisionEnabled(bEnableLeft ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void UWeaponContext::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	check(GetTypedOuter<AActor>()->HasAuthority());
	OnWeaponOverlapped.Broadcast(OtherActor);
}

void UWeaponContext::MulticastSetWeaponMesh_Implementation(UStaticMesh* RightMesh,
	const FTransform& RightTransform, UStaticMesh* LeftMesh, const FTransform& LeftTransform)
{
	RightWeapon->SetStaticMesh(RightMesh);
	RightWeapon->SetRelativeTransform(RightTransform);

	LeftWeapon->SetStaticMesh(LeftMesh);
	LeftWeapon->SetRelativeTransform(LeftTransform);
}

void UWeaponContext::Initialize(UStaticMeshComponent* InRightWeapon, class UStaticMeshComponent* InLeftWeapon)
{
	RightWeapon = InRightWeapon;
	LeftWeapon = InLeftWeapon;

	AActor* Outer = GetTypedOuter<AActor>();
	if (Outer && Outer->HasAuthority())
	{
		RightWeapon->OnComponentBeginOverlap.AddDynamic(this, &UWeaponContext::OnWeaponOverlap);
		LeftWeapon->OnComponentBeginOverlap.AddDynamic(this, &UWeaponContext::OnWeaponOverlap);
		SetWeaponCollision(false, false);
	}
	else
	{
		RightWeapon->SetCollisionProfileName(TEXT("NoCollision"));
		LeftWeapon->SetCollisionProfileName(TEXT("NoCollision"));

		RightWeapon->SetGenerateOverlapEvents(false);
		LeftWeapon->SetGenerateOverlapEvents(false);
	}
}
