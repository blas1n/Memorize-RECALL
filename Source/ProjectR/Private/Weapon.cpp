// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectRCharacter.h"
#include "ProjectRGameInstance.h"
#include "Skill.h"
#include "WeaponComponent.h"
#include "WeaponData.h"

void UWeapon::Initialize(const FName& InName)
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
	
	AsyncLoadCount = 2;
	AsyncLoad(RightWeaponMesh, WeaponData.RightMesh);
	AsyncLoad(LeftWeaponMesh, WeaponData.LeftMesh);

	Skills.SetNum(WeaponData.Skills.Num());

	for (int32 Index = 0; Index < WeaponData.Skills.Num(); ++Index)
	{
		TSubclassOf<USkill> SkillClass = WeaponData.Skills[Index];
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
	if (Skills.Num() > Index)
		Skills[Index]->Use();
}

bool UWeapon::CanUseSkill(uint8 Index)
{
	return Skills.Num() > Index ? Skills[Index]->CanUse() : false;
}

void UWeapon::SetWeaponCollision(bool bRightWeaponEnable, bool bLeftWeaponEnable)
{
	User->GetWeaponComponent()->SetWeaponCollision(bRightWeaponEnable, bLeftWeaponEnable);
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
