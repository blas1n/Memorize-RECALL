// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "Component/StatComponent.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"
#include "Data/SkillData.h"
#include "Data/WeaponData.h"
#include "Interface/Executable.h"
#include "Interface/StateExecutable.h"
#include "Library/PRStatics.h"
#include "Misc/Skill.h"

UWeapon::UWeapon()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponDataTableFinder(TEXT("DataTable'/Game/Data/DataTable/DT_WeaponData.DT_WeaponData'"));
	WeaponDataTable = WeaponDataTableFinder.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> SkillDataTableFinder(TEXT("DataTable'/Game/Data/DataTable/DT_SkillData.DT_SkillData'"));
	SkillDataTable = SkillDataTableFinder.Object;
}

bool UWeapon::Initialize(USkillContext* InContext, uint8 InKey)
{
	Context = InContext;
	Key = InKey;

	User = GetTypedOuter<APRCharacter>();
	if (!User)
	{
		Key = 0u;
		return false;
	}

	const auto* Data = WeaponDataTable->FindRow<FWeaponData>(FName{ *FString::FromInt(Key) }, TEXT(""), false);
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found weapon data %d!"), Key);
		Key = 0u;
		return false;
	}

	ComboDuration = Data->ComboDuration;
	WeakAttackClass = Data->WeakAttackClass;
	StrongAttackClass = Data->StrongAttackClass;

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

		if ((Index % 2) && WeakAttackClass)
			Skills[Index] = NewObject<USkill>(this, WeakAttackClass);
		else if (StrongAttackClass)
			Skills[Index] = NewObject<USkill>(this, StrongAttackClass);
	}

	VisualData.UpperAnimInstance = Data->UpperAnimInstance;
	VisualData.RightTransform = Data->RightTransform;
	VisualData.LeftTransform = Data->LeftTransform;

	LoadAll(*Data);
	return true;
}

void UWeapon::BeginPlay()
{
	User->GetStatComponent()->OnChangedLevel.AddUObject(this, &UWeapon::InitSkill);
}

void UWeapon::BeginSkill(uint8 Index)
{
	if (Skills.IsValidIndex(Index))
	{
		if (USkill* Skill = Skills[Index])
		{
			if (Skill->CanUseSkill())
			{
				Skill->Begin();
				return;
			}
		}
	}

	User->GetWeaponComponent()->OnEndSkill();
}

void UWeapon::EndSkill(uint8 Index)
{
	if (Skills.IsValidIndex(Index))
		if (USkill* Skill = Skills[Index])
			Skill->End();
}

void UWeapon::RegisterOnAsyncLoadEnded(const FOnAsyncLoadEndedSingle& Callback)
{
	check(Callback.IsBound());
	if (AsyncLoadCount) OnAsyncLoadEnded.Add(Callback);
	else Callback.Execute();
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

void UWeapon::TickExecute(uint8 Index, float DeltaSeconds)
{
	if (!Skills.IsValidIndex(Index)) return;

	USkill* Skill = Skills[Index];
	if (Skill && Skill->GetClass()->ImplementsInterface(UStateExecutable::StaticClass()))
		return IStateExecutable::Execute_TickExecute(Skill, DeltaSeconds);
}

void UWeapon::LoadAll(const FWeaponData& WeaponData)
{
	if (!WeaponData.RightMesh.IsNull()) ++AsyncLoadCount;
	if (!WeaponData.LeftMesh.IsNull()) ++AsyncLoadCount;

	UPRStatics::AsyncLoad(WeaponData.RightMesh, [this, &RightMeshPtr = WeaponData.RightMesh]
	{
		VisualData.RightMesh = RightMeshPtr.Get();
		if (--AsyncLoadCount == 0u)
			OnAsyncLoadEnded.Broadcast();
	});

	UPRStatics::AsyncLoad(WeaponData.LeftMesh, [this, &LeftMeshPtr = WeaponData.LeftMesh]
	{
		VisualData.LeftMesh = LeftMeshPtr.Get();
		if (--AsyncLoadCount == 0u)
			OnAsyncLoadEnded.Broadcast();
	});
}

void UWeapon::InitSkill(uint8 Level)
{
	if (Key == 0u || !SkillDataTable) return;

	const FString BaseKey = FString::FromInt(Key) + FString::FromInt(Level);
	const int32 SkillNum = Skills.Num();
	for (int32 Idx = 0; Idx < SkillNum; ++Idx)
	{
		USkill* Skill = Skills[Idx];
		if (!Skill) continue;

		const TSubclassOf<USkill> DefaultClass = Idx % 2 ? WeakAttackClass : StrongAttackClass;
		const int32 Prefix = ((Idx == 0) || Skill->GetClass() == DefaultClass) ? 0 : 1;

		int32 SkillIdx = Idx;
		if (Prefix == 0 && Idx != 0)
		{
			SkillIdx = (static_cast<int32>(FMath::Log2(Idx + 1)) - 1) * 2 + 1;
			if ((Idx % 2) == 0) ++SkillIdx;
		}

		const FName SkillKey{ *(BaseKey + FString::FromInt(Prefix) + FString::FromInt(SkillIdx)) };
		const auto* Data = SkillDataTable->FindRow<FSkillData>(SkillKey, TEXT(""), false);
		if (!Data)
		{
			UE_LOG(LogDataTable, Error, TEXT("Cannot found skill data %s!"), *SkillKey.ToString());
			continue;
		}

		Skill->Initialize(Context, Data ? Data->Data : nullptr);
	}
}
