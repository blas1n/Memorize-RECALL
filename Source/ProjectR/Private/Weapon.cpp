// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ProjectRCharacter.h"
#include "Data/WeaponData.h"
#include "Framework/ProjectRGameInstance.h"
#include "Skill.h"
#include "WeaponComponent.h"

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
	
	AsyncLoadCount = 3;
	AsyncLoad(RightWeaponMesh, WeaponData.RightMesh);
	AsyncLoad(LeftWeaponMesh, WeaponData.LeftMesh);
	AsyncLoad(EquipAnim, WeaponData.EquipAnim);

	Skills.SetNum(WeaponData.Skills.Num());

	for (int32 Index = 0; Index < WeaponData.Skills.Num(); ++Index)
	{
		TSubclassOf<USkill> SkillClass = WeaponData.Skills[Index];
		Skills[Index] = NewObject<USkill>(this, SkillClass);
		Skills[Index]->Initialize();
	}
}

void UWeapon::Release()
{
	for (USkill* Skill : Skills)
		Skill->Release();
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

void UWeapon::UseSkill(uint8 Index)
{
	if (Skills.Num() > Index)
		Skills[Index]->Use();
}

bool UWeapon::CanUseSkill(uint8 Index) const
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

void UWeapon::PlayEquipAnim()
{
	if (EquipAnim)
		User->PlayAnimMontage(EquipAnim);
}
