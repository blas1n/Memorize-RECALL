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
#include "Interface/ComponentOwner.h"
#include "Interface/Executable.h"
#include "Interface/StateExecutable.h"
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

	int32 SkillNum = 1;
	for (uint8 Idx = 1u; Idx <= Data->ComboHeight; ++Idx)
		SkillNum += static_cast<int32>(FMath::Pow(2, Idx));

	Skills.Init(nullptr, SkillNum);

	if (Data->ParryingClass)
		Skills[0] = NewObject<USkill>(this, Data->ParryingClass);

	for (const auto& Skill : Data->Skills)
		if (Skill.Value)
			Skills[Skill.Key] = NewObject<USkill>(this, Skill.Value);

	for (int32 Index = 1; Index < SkillNum; ++Index)
	{
		if (Skills[Index]) continue;

		if (Index % 2 && Data->WeakAttackClass)
			Skills[Index] = NewObject<USkill>(this, Data->WeakAttackClass);
		else if (Data->StrongAttackClass)
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
	auto* StatComp = IComponentOwner::Execute_GetStatComponent(User);
	StatComp->OnChangedLevel.AddUObject(this, &UWeapon::InitSkill);
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
		if (USkill* Skill = Skills[Index])
			Skill->Begin();
}

void UWeapon::EndSkill(uint8 Index)
{
	if (Skills.IsValidIndex(Index))
		if (USkill* Skill = Skills[Index])
			Skill->End();
}

void UWeapon::Execute(uint8 Index)
{
	if (!Skills.IsValidIndex(Index)) return;

	USkill* Skill = Skills[Index];
	if (Skill && Skill->GetClass()->ImplementsInterface(UExecutable::StaticClass()))
		return IExecutable::Execute_Execute(Skill);
}

void UWeapon::BeginExecute(uint8 Index)
{
	if (!Skills.IsValidIndex(Index)) return;

	USkill* Skill = Skills[Index];
	if (Skill && Skill->GetClass()->ImplementsInterface(UStateExecutable::StaticClass()))
		return IStateExecutable::Execute_BeginExecute(Skill);
}

void UWeapon::EndExecute(uint8 Index)
{
	if (!Skills.IsValidIndex(Index)) return;

	USkill* Skill = Skills[Index];
	if (Skill && Skill->GetClass()->ImplementsInterface(UStateExecutable::StaticClass()))
		return IStateExecutable::Execute_EndExecute(Skill);
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
		const FName SkillKey{ *(KeyStr + FString::FromInt(Idx) + LevelStr) };
		const auto* Data = SkillDataTable->FindRow<FSkillData>(SkillKey, TEXT(""), false);
		if (!Data) UE_LOG(LogDataTable, Error, TEXT("Cannot found skill data %s!"), *SkillKey.ToString());

		if (USkill* Skill = Skills[Idx])
			Skill->Initialize(Context, Data ? Data->Data : nullptr);
	}
}
