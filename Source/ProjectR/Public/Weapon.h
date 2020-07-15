// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnAsyncLoadEndedSingle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAsyncLoadEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnequipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExecute);

UCLASS(BlueprintType)
class PROJECTR_API UWeapon final : public UObject
{
	GENERATED_BODY()
	
public:
	void BeginPlay(const FName& InName);
	void EndPlay();

	void Equip();
	void Unequip();

	void StartSkill(uint8 Index);
	void EndSkill(uint8 Index); 
	bool CanUseSkill(uint8 Index) const;

	UFUNCTION(BlueprintCallable)
	void RegisterOnAsyncLoadEnded(const FOnAsyncLoadEndedSingle& Callback);

	FORCEINLINE const FName& GetName() const noexcept { return Name; }
	FORCEINLINE uint8 GetKey() const noexcept { return Key; }

	FORCEINLINE class UStaticMesh* GetRightWeaponMesh() const noexcept { return RightWeaponMesh; }
	FORCEINLINE const FTransform& GetRightWeaponTransform() const noexcept { return RightWeaponTransform; }

	FORCEINLINE UStaticMesh* GetLeftWeaponMesh() const noexcept { return LeftWeaponMesh; }
	FORCEINLINE const FTransform& GetLeftWeaponTransform() const noexcept { return LeftWeaponTransform; }

private:
	UFUNCTION()
	void PlayEquipAnim();

	void LoadAll(const struct FWeaponData& WeaponData);

	FORCEINLINE void CheckAndCallAsyncLoadDelegate() { if (--AsyncLoadCount == 0) OnAsyncLoadEnded.Broadcast(); }

public:
	UPROPERTY(BlueprintAssignable)
	FOnEquipped OnEquipped;

	UPROPERTY(BlueprintAssignable)
	FOnUnequipped OnUnequipped;

	UPROPERTY(BlueprintAssignable)
	FOnExecute OnExecute;

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

	FTransform RightWeaponTransform;
	FTransform LeftWeaponTransform;

	FOnAsyncLoadEnded OnAsyncLoadEnded;

	FName Name;
	uint8 Key;
	uint8 AsyncLoadCount;
};
