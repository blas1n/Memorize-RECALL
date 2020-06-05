// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
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

	Skills.SetNum(5);
	MeshLoadCount = 2;
}

void AWeapon::Initialize(const FWeaponData* WeaponData)
{
	Key = WeaponData->Key;
	Name = WeaponData->Name;

	LoadWeapon(LeftWeaponInfo, new TAssetPtr<UStaticMesh>{ WeaponData->LeftMesh }, WeaponData->LeftTransform);
	LoadWeapon(RightWeaponInfo, new TAssetPtr<UStaticMesh>{ WeaponData->RightMesh }, WeaponData->RightTransform);

	UWorld* World = GetWorld();
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = SpawnParam.Instigator = GetInstigator();
	SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	for (uint8 Index = 0; Index < 5; ++Index)
	{
		TSubclassOf<ASkill> SkillClass = WeaponData->Skills[Index];
		Skills[Index] = World->SpawnActor<ASkill>(SkillClass, SpawnParam);
		Skills[Index]->Initialize(this);
	}
}

void AWeapon::Equip()
{
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

void AWeapon::RegisterOnWeaponMeshLoaded(const FOnWeaponMeshLoadedSingle& Callback)
{
	check(Callback.IsBound());
	if (MeshLoadCount) OnWeaponMeshLoaded.Add(Callback);
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

void AWeapon::LoadWeapon(FWeaponInfo& WeaponInfo, TAssetPtr<UStaticMesh>* MeshPtr, const FTransform& Transform)
{
	if (MeshPtr->IsNull())
	{
		--MeshLoadCount;
		delete MeshPtr;
		return;
	}

	WeaponInfo.Transform = Transform;

	if (MeshPtr->IsPending())
	{
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			MeshPtr->ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda([this, &WeaponInfo = WeaponInfo, MeshPtr]() mutable
				{ OnMeshLoaded(WeaponInfo, MeshPtr); })
		);
	}
	else OnMeshLoaded(WeaponInfo, MeshPtr);
}

void AWeapon::OnMeshLoaded(FWeaponInfo& WeaponInfo, TAssetPtr<UStaticMesh>* MeshPtr)
{
	WeaponInfo.Mesh = MeshPtr->Get();
	delete MeshPtr;

	if (--MeshLoadCount == 0) OnWeaponMeshLoaded.Broadcast();
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