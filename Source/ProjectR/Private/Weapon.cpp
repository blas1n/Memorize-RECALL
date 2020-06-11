// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/BlendSpaceBase.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "AnimCastData.h"
#include "ProjectRCharacter.h"
#include "Skill.h"
#include "WeaponData.h"

AWeapon::AWeapon()
	: Super()
{
 	PrimaryActorTick.bCanEverTick = false;
	SetCanBeDamaged(false);

	EquipMontage = nullptr;
	AsyncLoadCount = 6;
}

void AWeapon::Initialize(const FWeaponData* WeaponData)
{
	Key = WeaponData->Key;
	Name = WeaponData->Name;

	LeftWeaponInfo.Transform = WeaponData->LeftTransform;
	RightWeaponInfo.Transform = WeaponData->RightTransform;

	AsyncLoad(LeftWeaponInfo.Mesh, WeaponData->LeftMesh);
	AsyncLoad(RightWeaponInfo.Mesh, WeaponData->RightMesh);
	AsyncLoad(LocomotionSpace, WeaponData->AnimData.LocomotionSpace);
	AsyncLoad(JumpStart, WeaponData->AnimData.JumpStart);
	AsyncLoad(JumpLoop, WeaponData->AnimData.JumpLoop);
	AsyncLoad(JumpEnd, WeaponData->AnimData.JumpEnd);
	AsyncLoad(DodgeMontage, WeaponData->AnimData.DodgeMontage);
	AsyncLoad(EquipMontage, WeaponData->AnimData.EquipMontage);

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

bool AWeapon::UseSkill(uint8 Index)
{
	if (Skills.Num() > Index)
		return Skills[Index]->UseSkill();
	return false;
}

bool AWeapon::CanUseSkill(uint8 Index)
{
	if (Skills.Num() > Index)
		return Skills[Index]->CanUseSkill();
	return false;
}

void AWeapon::BeginSkill(UAnimMontage* Montage)
{
	AProjectRCharacter* User = Cast<AProjectRCharacter>(GetInstigator());

	for (const UAnimMetaData* Data : Montage->GetMetaData())
	{
		if (const UAnimCastData* CastData = Cast<UAnimCastData>(Data))
		{
			User->SetIsCasting(CastData->IsCasting());
			User->SetCanMoving(CastData->CanMoving());
			break;
		}
	}

	OnBeginSkill.Broadcast();
}

void AWeapon::EndSkill(UAnimMontage* Montage, bool bInterrupted)
{
	AProjectRCharacter* User = Cast<AProjectRCharacter>(GetInstigator());
	User->SetIsCasting(false);
	User->SetCanMoving(true);

	OnEndSkill.Broadcast();
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