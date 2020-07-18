// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Weapon.generated.h"

DECLARE_DELEGATE(FOnAsyncLoadEndedSingle);
DECLARE_MULTICAST_DELEGATE(FOnAsyncLoadEnded);

UCLASS(BlueprintType)
class PROJECTR_API UWeapon final : public UObject
{
	GENERATED_BODY()
	
public:
	UWeapon();
	void Initialize(int32 Key);

	void BeginPlay();
	void EndPlay();

	void Equip();
	void Unequip();

	void StartSkill(uint8 Index);
	void EndSkill(uint8 Index); 
	bool CanUseSkill(uint8 Index) const;

private:
	void LoadAll(const struct FWeaponData& WeaponData);

	FORCEINLINE void CheckAndCallAsyncLoadDelegate() { if (--AsyncLoadCount == 0) OnAsyncLoadEnded.Broadcast(); }

private:
	UPROPERTY()
	class AProjectRCharacter* User;

	UPROPERTY()
	TArray<class USkill*> Skills;

	UPROPERTY()
	TSubclassOf<class UAnimInstance> UpperAnimInstance;

	UPROPERTY()
	class UAnimMontage* EquipAnim;

	UPROPERTY()
	class UStaticMesh* RightWeaponMesh;

	UPROPERTY()
	UStaticMesh* LeftWeaponMesh;

	UPROPERTY()
	class UDataTable* WeaponDataTable;

	FTransform RightWeaponTransform;
	FTransform LeftWeaponTransform;

	FOnAsyncLoadEnded OnAsyncLoadEnded;

	uint8 AsyncLoadCount;
};
