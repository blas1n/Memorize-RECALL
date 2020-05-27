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
}

void AWeapon::Initialize(const FWeaponData* WeaponData)
{
	Key = WeaponData->Key;
	Name = WeaponData->Name;

	if (WeaponData->LeftMesh.IsPending())++MeshLoadCount;
	if (WeaponData->RightMesh.IsPending()) ++MeshLoadCount;

	LoadWeapon(LeftWeaponInfo, WeaponData->LeftMesh, WeaponData->LefttTransform);
	LoadWeapon(RightWeaponInfo, WeaponData->RightMesh, WeaponData->RightTransform);

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
	EquipOnce(RightWeapon, LeftWeaponInfo);

	RightWeapon->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnLeftWeaponOverlapped);
	RightWeapon->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnRightWeaponOverlapped);

	OnActive.Broadcast();
}

void AWeapon::Unequip()
{
	UnequipOnce(LeftWeapon);
	UnequipOnce(RightWeapon);

	OnInactive.Broadcast();
}

void AWeapon::PressSkill(uint8 Index)
{
	Skills[Index]->OnPress();
}

void AWeapon::ReleaseSkill(uint8 Index)
{
	Skills[Index]->OnRelease();
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	AProjectRCharacter* Character = Cast<AProjectRCharacter>(GetInstigator());
	LeftWeapon = Character->GetLeftWeapon();
	RightWeapon = Character->GetRightWeapon();
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

void AWeapon::LoadWeapon(FWeaponInfo& WeaponInfoRef, TAssetPtr<UStaticMesh> Mesh, const FTransform& Transform)
{
	if (!Mesh.IsPending()) return;

	const FSoftObjectPath& MeshPath = Mesh.ToSoftObjectPath();
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	Manager.RequestAsyncLoad(Mesh.ToSoftObjectPath(), FStreamableDelegate::CreateLambda(
		[this, WeaponInfoRef, Mesh]() mutable { OnMeshLoaded(WeaponInfoRef, Mesh); }));

	WeaponInfoRef.Transform = Transform;
}

void AWeapon::OnMeshLoaded(FWeaponInfo& WeaponInfoRef, TAssetPtr<UStaticMesh> Mesh)
{
	WeaponInfoRef.Mesh = Mesh.Get();

	if (--MeshLoadCount == 0) OnWeaponMeshLoaded.Broadcast();
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