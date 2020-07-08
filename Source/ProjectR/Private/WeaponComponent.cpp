// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ProjectRCharacter.h"
#include "Weapon.h"

UWeaponComponent::UWeaponComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = false;

	RightWeapon = CreateWeaponComponent(TEXT("RightWeapon"));
	LeftWeapon = CreateWeaponComponent(TEXT("LeftWeapon"));

	User = nullptr;
	WeaponNum = 0;
	CurIndex = 0;
}

void UWeaponComponent::StartSkill(uint8 Index)
{
	Weapons[CurIndex]->StartSkill(Index);
}

void UWeaponComponent::EndSkill(uint8 Index)
{
	Weapons[CurIndex]->EndSkill(Index);
}

bool UWeaponComponent::CanUseSkill(uint8 Index) const
{
	return Weapons[CurIndex]->CanUseSkill(Index);
}

void UWeaponComponent::SwapWeapon(uint8 Index)
{
	if (Index >= Weapons.Num() || CurIndex == Index)
		return;
	
	EquipWeapon(Weapons[Index], true);
	CurIndex = Index;
}

void UWeaponComponent::CreateNewWeapon(FName Name, uint8 Index)
{
	auto* NewWeapon = NewObject<UWeapon>(User);
	NewWeapon->BeginPlay(Name);

	EquipWeapon(NewWeapon, CurIndex == Index);

	Weapons[Index]->EndPlay();
	Weapons[Index] = NewWeapon;
}

void UWeaponComponent::SetWeaponCollision(bool bEnableRight, bool bEnableLeft)
{
	RightWeapon->SetGenerateOverlapEvents(bEnableRight);
	LeftWeapon->SetGenerateOverlapEvents(bEnableLeft);
}

void UWeaponComponent::BeginPlay()
{
	User = Cast<AProjectRCharacter>(GetOwner());
	User->OnDeath.AddDynamic(this, &UWeaponComponent::Detach);

	auto* MeshComponent = User->GetMesh();
	const auto Rules = FAttachmentTransformRules::KeepWorldTransform;
	RightWeapon->AttachToComponent(MeshComponent, Rules, TEXT("Weapon_r"));
	LeftWeapon->AttachToComponent(MeshComponent, Rules, TEXT("Weapon_l"));

	const TArray<FName> WeaponNames = User->GetWeaponNames();
	WeaponNum = WeaponNames.Num();
	Weapons.Init(nullptr, WeaponNum);

	for (int32 Idx = 0; Idx < WeaponNum; ++Idx)
	{
		Weapons[Idx] = NewObject<UWeapon>(User);
		Weapons[Idx]->BeginPlay(WeaponNames[Idx]);
	}

	if (Weapons.Num() > 0)
		EquipWeapon(Weapons[0], false);
	Super::BeginPlay();
}

void UWeaponComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	for (UWeapon* Weapon : Weapons)
		if (Weapon)	Weapon->EndPlay();

	Super::EndPlay(EndPlayReason);
}

UStaticMeshComponent* UWeaponComponent::CreateWeaponComponent(FName Name)
{
	auto* Component = CreateDefaultSubobject<UStaticMeshComponent>(Name);
	check(Component);

	Component->SetMobility(EComponentMobility::Movable);
	Component->OnComponentBeginOverlap.AddDynamic(this, &UWeaponComponent::OnWeaponOverlapped);

	Component->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	Component->SetCollisionProfileName(TEXT("Weapon"));
	Component->SetGenerateOverlapEvents(false);

	return Component;
}

void UWeaponComponent::EquipWeapon(UWeapon* NewWeapon, bool bNeedUnequip)
{
	if (!NewWeapon) return;

	if (bNeedUnequip)
		Weapons[CurIndex]->Unequip();

	NewWeapon->Equip();

	FOnAsyncLoadEndedSingle Callback;
	Callback.BindDynamic(this, &UWeaponComponent::SetWeaponMesh);
	NewWeapon->RegisterOnAsyncLoadEnded(Callback);
}

void UWeaponComponent::OnWeaponOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Weapons[CurIndex]->OnWeaponHitted.Broadcast(Cast<AProjectRCharacter>(OtherActor));
}

void UWeaponComponent::SetWeaponMesh()
{
	RightWeapon->SetStaticMesh(Weapons[CurIndex]->GetRightWeaponMesh());
	RightWeapon->SetRelativeTransform(Weapons[CurIndex]->GetRightWeaponTransform());

	LeftWeapon->SetStaticMesh(Weapons[CurIndex]->GetLeftWeaponMesh());
	LeftWeapon->SetRelativeTransform(Weapons[CurIndex]->GetLeftWeaponTransform());
}

void UWeaponComponent::Detach(AController* Instigator)
{
	DetachOnce(RightWeapon);
	DetachOnce(LeftWeapon);
}

void UWeaponComponent::DetachOnce(UStaticMeshComponent* Weapon)
{
	Weapon->SetCollisionProfileName(TEXT("Ragdoll"));
	Weapon->SetSimulatePhysics(true);

	auto Rules = FDetachmentTransformRules::KeepWorldTransform;
	Weapon->DetachFromComponent(Rules);
}
