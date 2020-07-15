// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/ActorComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ProjectRCharacter.h"
#include "Data/SkillData.h"
#include "Data/WeaponData.h"
#include "Framework/ProjectRGameInstance.h"
#include "ProjectRStatics.h"
#include "Skill.h"
#include "WeaponComponent.h"

void UWeapon::BeginPlay(const FName& InName)
{
	User = Cast<AProjectRCharacter>(GetOuter());
	Name = InName;

	const auto* GameInstance = Cast<UProjectRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	const auto* DataTable = GameInstance->GetDataTable(TEXT("WeaponData"));
	const auto& WeaponData = *DataTable->FindRow<FWeaponData>(Name, "", false);

	Key = WeaponData.Key;
	UpperAnimInstance = WeaponData.UpperAnimInstance;
	RightWeaponTransform = WeaponData.RightTransform;
	LeftWeaponTransform = WeaponData.LeftTransform;
	
	LoadAll(WeaponData);

	const int32 SkillNum = WeaponData.SkillsData.Num();
	Skills.Init(nullptr, SkillNum);
	for (int32 Index = 0; Index < SkillNum; ++Index)
	{
		const FSkillData& SkillData = WeaponData.SkillsData[Index];
		TSubclassOf<USkill> SkillClass = SkillData.SkillClass;
		Skills[Index] = NewObject<USkill>(this, SkillClass);
		Skills[Index]->BeginPlay(SkillData);
	}
}

void UWeapon::EndPlay()
{
	for (USkill* Skill : Skills)
		Skill->EndPlay();
}

void UWeapon::Equip()
{
	auto* AnimInstance = User->GetMesh()->GetAnimInstance();
	AnimInstance->LinkAnimClassLayers(UpperAnimInstance);
	OnEquipped.Broadcast();

	FOnAsyncLoadEndedSingle Callback;
	Callback.BindDynamic(this, &UWeapon::PlayEquipAnim);
	RegisterOnAsyncLoadEnded(Callback);
}

void UWeapon::Unequip()
{
	auto* AnimInstance = User->GetMesh()->GetAnimInstance();
	AnimInstance->UnlinkAnimClassLayers(UpperAnimInstance);
	OnUnequipped.Broadcast();
}

void UWeapon::StartSkill(uint8 Index)
{
	if (CanUseSkill(Index))
		Skills[Index]->Start();
}

void UWeapon::EndSkill(uint8 Index)
{
	if (Skills.Num() > Index)
		Skills[Index]->End();
}

bool UWeapon::CanUseSkill(uint8 Index) const
{
	if (Skills.Num() <= Index) return false;
	return Skills[Index]->CanUse();
}

void UWeapon::RegisterOnAsyncLoadEnded(const FOnAsyncLoadEndedSingle& Callback)
{
	check(Callback.IsBound());
	if (AsyncLoadCount) OnAsyncLoadEnded.Add(Callback);
	else Callback.Execute();
}

void UWeapon::PlayEquipAnim()
{
	if (EquipAnim)
		User->PlayAnimMontage(EquipAnim);
}

void UWeapon::LoadAll(const FWeaponData& WeaponData)
{
	if (!WeaponData.RightMesh.IsNull())
	{
		++AsyncLoadCount;
		UProjectRStatics::AsyncLoad(WeaponData.RightMesh, [this, &RightMeshPtr = WeaponData.RightMesh]() mutable
		{
			RightWeaponMesh = RightMeshPtr.Get();
			CheckAndCallAsyncLoadDelegate();
		});
	}

	if (!WeaponData.LeftMesh.IsNull())
	{
		++AsyncLoadCount;
		UProjectRStatics::AsyncLoad(WeaponData.LeftMesh, [this, &LeftMeshPtr = WeaponData.LeftMesh]() mutable
		{
			LeftWeaponMesh = LeftMeshPtr.Get();
			CheckAndCallAsyncLoadDelegate();
		});
	}

	if (!WeaponData.EquipAnim.IsNull())
	{
		++AsyncLoadCount;
		UProjectRStatics::AsyncLoad(WeaponData.EquipAnim, [this, &EquipAnimPtr = WeaponData.EquipAnim]() mutable
		{
			EquipAnim = EquipAnimPtr.Get();
			CheckAndCallAsyncLoadDelegate();
		});
	}
}
