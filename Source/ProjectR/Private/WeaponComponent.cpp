// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Character/ProjectRCharacter.h"
#include "Weapon.h"

UWeaponComponent::UWeaponComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = false;

	RightWeapon = CreateWeaponComponent(TEXT("RightWeapon"), TEXT("weapon_r"));
	LeftWeapon = CreateWeaponComponent(TEXT("LeftWeapon"), TEXT("weapon_l"));

	User = nullptr;
	WeaponNum = 0;
	CurIndex = 0;
}

void UWeaponComponent::Initialize(const TArray<int32>& Keies)
{
	WeaponNum = Keies.Num();
	Weapons.SetNum(WeaponNum);

	for (int32 Idx = 0; Idx < WeaponNum; ++Idx)
	{
		Weapons[Idx] = NewObject<UWeapon>(GetOwner());
		Weapons[Idx]->Initialize(Keies[Idx]);
	}

	if (Weapons.Num() > 0)
		EquipWeapon(Weapons[0], false);
}

void UWeaponComponent::StartSkill(uint8 Index)
{
	if (WeaponNum > 0)
		Weapons[CurIndex]->StartSkill(Index);
}

void UWeaponComponent::EndSkill(uint8 Index)
{
	if (WeaponNum > 0)
		Weapons[CurIndex]->EndSkill(Index);
}

bool UWeaponComponent::CanUseSkill(uint8 Index) const
{
	if (WeaponNum == 0) return false;
	return Weapons[CurIndex]->CanUseSkill(Index);
}

void UWeaponComponent::SwapWeapon(uint8 Index)
{
	if (Index >= Weapons.Num() || CurIndex == Index)
		return;
	
	EquipWeapon(Weapons[Index], true);
	CurIndex = Index;
}

void UWeaponComponent::CreateNewWeapon(int32 Key, uint8 Index)
{
	auto* NewWeapon = NewObject<UWeapon>(User);
	NewWeapon->Initialize(Key);
	NewWeapon->BeginPlay();

	EquipWeapon(NewWeapon, CurIndex == Index);

	Weapons[Index]->EndPlay();
	Weapons[Index] = NewWeapon;
}

void UWeaponComponent::BeginPlay()
{
	User = Cast<AProjectRCharacter>(GetOwner());
	User->OnDeath.AddDynamic(this, &UWeaponComponent::Detach);

	Super::BeginPlay();

	for (UWeapon* Weapon : Weapons)
		if (Weapon)	Weapon->BeginPlay();
}

void UWeaponComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	for (UWeapon* Weapon : Weapons)
		if (Weapon)	Weapon->EndPlay();

	Super::EndPlay(EndPlayReason);
}

UStaticMeshComponent* UWeaponComponent::CreateWeaponComponent(const FName& Name, const FName& SocketName)
{
	auto* Component = CreateDefaultSubobject<UStaticMeshComponent>(Name);
	check(Component);

	Component->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	Component->SetMobility(EComponentMobility::Movable);
	Component->SetCollisionProfileName(TEXT("Weapon"));
	Component->SetGenerateOverlapEvents(false);

	auto* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		auto* MeshComponent = Character->GetMesh();
		const auto Rules = FAttachmentTransformRules::KeepRelativeTransform;
		if (MeshComponent->DoesSocketExist(SocketName))
			Component->AttachToComponent(MeshComponent, Rules, SocketName);
	}

	return Component;
}

void UWeaponComponent::EquipWeapon(UWeapon* NewWeapon, bool bNeedUnequip)
{
	if (!NewWeapon) return;

	if (bNeedUnequip)
		Weapons[CurIndex]->Unequip();

	NewWeapon->Equip();
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
