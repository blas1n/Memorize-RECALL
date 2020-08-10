// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon/Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Component/StatComponent.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"
#include "Data/SkillData.h"
#include "Data/WeaponData.h"
#include "Library/PRStatics.h"
#include "Weapon/Skill.h"
#include "Weapon/WeaponContext.h"

UWeapon::UWeapon()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableFinder(TEXT("DataTable'/Game/Data/DataTable/DT_WeaponData.DT_WeaponData'"));
	WeaponDataTable = WeaponDataTableFinder.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> SkillDataTableFinder(TEXT("DataTable'/Game/Data/DataTable/DT_SkillData.DT_SkillData'"));
	SkillDataTable = SkillDataTableFinder.Object;
}

bool UWeapon::Initialize(UWeaponContext* InContext, int32 InKey)
{
	Context = InContext;
	Key = InKey;

	User = GetTypedOuter<APRCharacter>();
	if (!User)
	{
		Key = -1;
		return false;
	}

	const auto* Data = WeaponDataTable->FindRow<FWeaponData>(FName{ *FString::FromInt(Key) }, TEXT(""), false);
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found weapon data %d!"), Key);
		Key = -1;
		return false;
	}

	if (!Data->WeakAttackClass || !Data->StrongAttackClass)
	{
		UE_LOG(LogDataTable, Error, TEXT("Attack class is not valid!"), Key);
		Key = -1;
		return false;
	}

	int32 SkillNum = 0;
	for (uint8 Idx = 1u; Idx <= Data->ComboHeight; ++Idx)
		SkillNum += static_cast<int32>(FMath::Pow(2, Idx));

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
	return true;
}

void UWeapon::BeginPlay()
{
	auto* StatComp = User->GetStatComponent();
	StatComp->OnChangedLevel.AddUObject(this, &UWeapon::InitSkill);
	InitSkill(StatComp->GetLevel());
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

void UWeapon::InitSkill(uint8 Level)
{
	if (Key == -1) return;

	const FString KeyStr = FString::FromInt(Key);
	const FString LevelStr = FString::FromInt(Level);

	const int32 SkillNum = Skills.Num();
	for (int32 Idx = 0; Idx < SkillNum; ++Idx)
	{
		const auto* Data = SkillDataTable->FindRow<FSkillData>(FName{ *(KeyStr + FString::FromInt(Idx + 1) + LevelStr) }, TEXT(""), false);
		if (!Data)
		{
			UE_LOG(LogDataTable, Error, TEXT("Cannot found weapon data %d!"), Key);
			return;
		}

		Skills[Idx]->Initialize(Context, Data->Data);
	}

	const auto* Data = SkillDataTable->FindRow<FSkillData>(FName{ *(KeyStr + TEXT("0") + LevelStr) }, TEXT(""), false);
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found weapon data %d!"), Key);
		return;
	}

	Parrying->Initialize(Context, Data->Data);
}
