// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/ProjectRCharacter.h"
#include "Data/WeaponData.h"
#include "ProjectRStatics.h"
#include "Skill.h"

UWeapon::UWeapon()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_WeaponData.DT_WeaponData'"));
	WeaponDataTable = DataTable.Object;
}

void UWeapon::Initialize(int32 Key)
{
	User = GetTypedOuter<AProjectRCharacter>();
	if (!User) return;

	const auto* Data = WeaponDataTable->FindRow<FWeaponData>(FName{ *FString::FromInt(Key) }, TEXT(""));
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found weapon data %d!"), Key);
		return;
	}

	const int32 SkillNum = Data->SkillClasses.Num();
	Skills.SetNum(SkillNum);

	for (int32 Index = 0; Index < SkillNum; ++Index)
	{
		TSubclassOf<USkill> SkillClass = Data->SkillClasses[Index];
		Skills[Index] = NewObject<USkill>(this, SkillClass);

		FString KeyStr = FString::FromInt(Key);
		KeyStr += FString::FromInt(Index);
		KeyStr += FString::FromInt(User->GetLevel());

		Skills[Index]->Initialize(FName{ *KeyStr });
	}

	UpperAnimInstance = Data->UpperAnimInstance;
	RightWeaponTransform = Data->RightTransform;
	LeftWeaponTransform = Data->LeftTransform;

	LoadAll(*Data);
}

void UWeapon::BeginPlay()
{
	for (USkill* Skill : Skills)
		Skill->BeginPlay();
}

void UWeapon::EndPlay()
{
	for (USkill* Skill : Skills)
		Skill->EndPlay();
}

void UWeapon::Equip()
{
	if (auto* AnimInstance = User->GetMesh()->GetAnimInstance())
		AnimInstance->LinkAnimClassLayers(UpperAnimInstance);

	OnEquipped.Broadcast();

	RegisterOnAsyncLoadEnded(FOnAsyncLoadEndedSingle::CreateLambda([this]
	{
		if (EquipAnim)
			User->PlayAnimMontage(EquipAnim);
	}));
}

void UWeapon::Unequip()
{
	if (auto* AnimInstance = User->GetMesh()->GetAnimInstance())
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
