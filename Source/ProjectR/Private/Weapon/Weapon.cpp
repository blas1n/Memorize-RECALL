// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"
#include "Data/WeaponData.h"
#include "Library/PRStatics.h"
#include "Weapon/Skill.h"
#include "Weapon/WeaponContext.h"

UWeapon::UWeapon()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_WeaponData.DT_WeaponData'"));
	WeaponDataTable = DataTable.Object;
}

void UWeapon::Initialize(UWeaponContext* InContext, int32 Key)
{
	Context = InContext;

	User = GetTypedOuter<APRCharacter>();
	if (!User) return;

	const auto* Data = WeaponDataTable->FindRow<FWeaponData>(FName{ *FString::FromInt(Key) }, TEXT(""));
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found weapon data %d!"), Key);
		return;
	}

	if (!Data->WeakAttackClass || !Data->StrongAttackClass)
	{
		UE_LOG(LogDataTable, Error, TEXT("Attack class is not valid!"), Key);
		return;
	}

	int32 SkillNum = 0;
	for (uint8 Idx = 1u; Idx <= Data->ComboHeight; ++Idx)
		SkillNum += static_cast<int>(pow(2, Idx));

	Skills.Init(nullptr, SkillNum);

	for (const auto& Skill : Data->Skills)
		Skills[Skill.Key] = NewObject<USkill>(this, Skill.Value);

	for (int32 Index = 0; Index < SkillNum; ++Index)
	{
		if (Skills[Index]) continue;

		if (Index % 2)
			Skills[Index] = NewObject<USkill>(this, Data->WeakAttackClass);
		else
			Skills[Index] = NewObject<USkill>(this, Data->StrongAttackClass);
	}

	UpperAnimInstance = Data->UpperAnimInstance;
	RightWeaponTransform = Data->RightTransform;
	LeftWeaponTransform = Data->LeftTransform;

	LoadAll(*Data);
}

void UWeapon::Equip()
{
	if (auto* AnimInstance = User->GetMesh()->GetAnimInstance())
		AnimInstance->LinkAnimClassLayers(UpperAnimInstance);

	const auto SetWeaponMesh = [this]
	{
		Context->SetWeaponMesh(RightWeaponMesh, RightWeaponTransform, LeftWeaponMesh, LeftWeaponTransform);
		if (EquipAnim) User->PlayAnimMontage(EquipAnim);
	};

	if (AsyncLoadCount)
		OnAsyncLoadEnded.AddLambda(SetWeaponMesh);
	else SetWeaponMesh();
}

void UWeapon::Unequip()
{
	if (auto* AnimInstance = User->GetMesh()->GetAnimInstance())
		AnimInstance->UnlinkAnimClassLayers(UpperAnimInstance);
}

void UWeapon::BeginSkill(uint8 Index)
{
	if (Skills.IsValidIndex(Index))
		Skills[Index]->Begin();
}

void UWeapon::EndSkill(uint8 Index)
{
	if (Skills.IsValidIndex(Index))
		Skills[Index]->End();
}

void UWeapon::BeginParrying()
{
	if (Parrying) Parrying->Begin();
}

void UWeapon::EndParrying()
{
	if (Parrying) Parrying->End();
}

void UWeapon::LoadAll(const FWeaponData& WeaponData)
{
	if (!WeaponData.RightMesh.IsNull())
	{
		++AsyncLoadCount;
		UPRStatics::AsyncLoad(WeaponData.RightMesh, [this, &RightMeshPtr = WeaponData.RightMesh]
		{
			RightWeaponMesh = RightMeshPtr.Get();
			if (--AsyncLoadCount == 0)
				OnAsyncLoadEnded.Broadcast();
		});
	}

	if (!WeaponData.LeftMesh.IsNull())
	{
		++AsyncLoadCount;
		UPRStatics::AsyncLoad(WeaponData.LeftMesh, [this, &LeftMeshPtr = WeaponData.LeftMesh]
		{
			LeftWeaponMesh = LeftMeshPtr.Get();
			if (--AsyncLoadCount == 0)
				OnAsyncLoadEnded.Broadcast();
		});
	}

	if (!WeaponData.EquipAnim.IsNull())
	{
		++AsyncLoadCount;
		UPRStatics::AsyncLoad(WeaponData.EquipAnim, [this, &EquipAnimPtr = WeaponData.EquipAnim]
		{
			EquipAnim = EquipAnimPtr.Get();
			if (--AsyncLoadCount == 0)
				OnAsyncLoadEnded.Broadcast();
		});
	}
}
