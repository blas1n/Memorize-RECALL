// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/World.h"
#include "ProjectRCharacter.h"
#include "Skill.h"
#include "WeaponData.h"

AWeapon::AWeapon()
	: Super()
{
 	PrimaryActorTick.bCanEverTick = false;
	SetCanBeDamaged(false);
	AsyncLoadCount = 3;
	EquipMontage = nullptr;
}

void AWeapon::Initialize(const FWeaponData* WeaponData)
{
	Key = WeaponData->Key;
	Name = WeaponData->Name;

	LoadWeapon(LeftWeaponInfo, WeaponData->LeftMesh, WeaponData->LeftTransform);
	LoadWeapon(RightWeaponInfo, WeaponData->RightMesh, WeaponData->RightTransform);

	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = GetOwner();
	SpawnParam.Instigator = GetInstigator();
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	Skills.SetNum(WeaponData->Skills.Num());

	for (int32 Index = 0; Index < WeaponData->Skills.Num(); ++Index)
	{
		TSubclassOf<ASkill> SkillClass = WeaponData->Skills[Index];
		Skills[Index] = GetWorld()->SpawnActor<ASkill>(SkillClass, SpawnParam);
		Skills[Index]->Initialize(this);
	}

	if (!WeaponData->EquipMontage.IsNull())
	{
		if (--AsyncLoadCount == 0) OnAsyncLoadEnded.Broadcast();
		return;
	}

	const TAssetPtr<UAnimMontage>& EquipMontagePtr = WeaponData->EquipMontage;

	if (EquipMontagePtr.IsPending())
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			EquipMontagePtr.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([this, EquipMontagePtr]() mutable
				{ OnEquipMontageLoaded(EquipMontagePtr); })
		);
	}
	else OnEquipMontageLoaded(EquipMontagePtr);
}

void AWeapon::Equip()
{
	if (EquipMontage)
		Cast<ACharacter>(GetInstigator())->PlayAnimMontage(EquipMontage);

	EquipOnce(LeftWeapon, LeftWeaponInfo);
	EquipOnce(RightWeapon, RightWeaponInfo);

	LeftWeapon->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnLeftWeaponOverlapped);
	RightWeapon->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnRightWeaponOverlapped);

	OnActive.Broadcast();
}

void AWeapon::Unequip()
{
	UnequipOnce(LeftWeapon);
	UnequipOnce(RightWeapon);

	LeftWeapon->OnComponentBeginOverlap.RemoveDynamic(this, &AWeapon::OnLeftWeaponOverlapped);
	RightWeapon->OnComponentBeginOverlap.RemoveDynamic(this, &AWeapon::OnRightWeaponOverlapped);

	OnInactive.Broadcast();
}

void AWeapon::UseSkill(uint8 Index)
{
	Skills[Index]->UseSkill();
}

void AWeapon::RegisterOnAsyncLoadEnded(const FOnAsyncLoadEndedSingle& Callback)
{
	check(Callback.IsBound());
	if (AsyncLoadCount) OnAsyncLoadEnded.Add(Callback);
	else Callback.Execute();
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	AProjectRCharacter* Character = Cast<AProjectRCharacter>(GetInstigator());
	LeftWeapon = Character->GetLeftWeapon();
	RightWeapon = Character->GetRightWeapon();

	Character->GetMesh()->GetAnimInstance()->OnMontageStarted.AddDynamic(this, &AWeapon::BeginSkill);
	Character->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AWeapon::EndSkill);
}

void AWeapon::EquipOnce(UStaticMeshComponent* Weapon, const FWeaponInfo& Info)
{
	if (!Info.Mesh) return;

	Weapon->SetStaticMesh(Info.Mesh);
	Weapon->SetRelativeTransform(Info.Transform);
}

void AWeapon::UnequipOnce(UStaticMeshComponent* Weapon)
{
	Weapon->SetStaticMesh(nullptr);
	Weapon->SetRelativeTransform(FTransform{});
}

void AWeapon::LoadWeapon(FWeaponInfo& WeaponInfo, const TAssetPtr<UStaticMesh>& MeshPtr, const FTransform& Transform)
{
	if (MeshPtr.IsNull())
	{
		if (--AsyncLoadCount == 0) OnAsyncLoadEnded.Broadcast();
		return;
	}

	WeaponInfo.Transform = Transform;

	if (MeshPtr.IsPending())
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			MeshPtr.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([this, &WeaponInfo = WeaponInfo, &MeshPtr = MeshPtr]() mutable
				{ OnMeshLoaded(WeaponInfo, MeshPtr); })
		);
	}
	else OnMeshLoaded(WeaponInfo, MeshPtr);
}

void AWeapon::OnMeshLoaded(FWeaponInfo& WeaponInfo, const TAssetPtr<UStaticMesh>& MeshPtr)
{
	WeaponInfo.Mesh = MeshPtr.Get();
	if (--AsyncLoadCount == 0) OnAsyncLoadEnded.Broadcast();
}

void AWeapon::OnEquipMontageLoaded(const TAssetPtr<UAnimMontage>& MontagePtr)
{
	EquipMontage = MontagePtr.Get();
	if (--AsyncLoadCount == 0) OnAsyncLoadEnded.Broadcast();
}

void AWeapon::BeginSkill(UAnimMontage* Montage)
{
	OnBeginSkill.Broadcast();
}

void AWeapon::EndSkill(UAnimMontage* Montage, bool bInterrupted)
{
	OnEndSkill.Broadcast();
}

void AWeapon::OnLeftWeaponOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnLeftWeaponHitted.Broadcast(OtherActor);
}

void AWeapon::OnRightWeaponOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnRightWeaponHitted.Broadcast(OtherActor);
}