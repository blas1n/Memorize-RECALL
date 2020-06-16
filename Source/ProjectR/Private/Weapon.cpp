// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StaticMesh.h"
#include "ProjectRCharacter.h"
#include "Skill.h"
#include "WeaponData.h"

void UWeapon::Initialize(const FName& InName, const FWeaponData& Data)
{
	User = Cast<AProjectRCharacter>(GetOuter());

	Name = InName;
	Key = Data.Key;
	UpperAnimInstance = Data.UpperAnimInstance;
	RightWeaponTransform = Data.RightTransform;
	LeftWeaponTransform = Data.LeftTransform;
	
	AsyncLoadCount = 2;
	AsyncLoad(RightWeaponMesh, Data.RightMesh);
	AsyncLoad(LeftWeaponMesh, Data.LeftMesh);

	Skills.SetNum(Data.Skills.Num());

	for (int32 Index = 0; Index < Data.Skills.Num(); ++Index)
	{
		TSubclassOf<USkill> SkillClass = Data.Skills[Index];
		Skills[Index] = NewObject<USkill>(this, SkillClass);
		Skills[Index]->Initialize();
	}
}

void UWeapon::Equip()
{
	auto* AnimInstance = User->GetMesh()->GetAnimInstance();
	AnimInstance->LinkAnimClassLayers(UpperAnimInstance);
	OnEquipped.Broadcast();
}

void UWeapon::Unequip()
{
	auto* AnimInstance = User->GetMesh()->GetAnimInstance();
	AnimInstance->UnlinkAnimClassLayers(UpperAnimInstance);
	OnUnequipped.Broadcast();
}

void UWeapon::UseSkill(uint8 Index)
{
	USkill* Skill = Skills[Index];
	if (Skill) Skill->Use();
}

bool UWeapon::CanUseSkill(uint8 Index)
{
	USkill* Skill = Skills[Index];
	return Skill ? Skill->CanUse() : false;
}

void UWeapon::RegisterOnAsyncLoadEnded(const FOnAsyncLoadEndedSingle& Callback)
{
	check(Callback.IsBound());
	if (AsyncLoadCount) OnAsyncLoadEnded.Add(Callback);
	else Callback.Execute();
}

void UWeapon::AsyncLoad(UStaticMesh*& Ptr, const TAssetPtr<UStaticMesh>& SoftPtr)
{
	if (SoftPtr.IsNull())
	{
		CheckAndCallAsyncLoadDelegate();
		return;
	}

	auto OnAsyncLoaded = [this, &Ptr = Ptr, &SoftPtr = SoftPtr]() mutable
	{
		Ptr = SoftPtr.Get();
		CheckAndCallAsyncLoadDelegate();
	};

	if (SoftPtr.IsPending())
	{
		FStreamableDelegate Callback;
		Callback.BindLambda([this, OnAsyncLoaded = MoveTemp(OnAsyncLoaded)]() mutable
		{
			OnAsyncLoaded();
		});

		UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftPtr.ToSoftObjectPath(), MoveTemp(Callback));
	}
	else OnAsyncLoaded();
}
