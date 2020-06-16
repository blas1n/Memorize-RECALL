// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnAsyncLoadEndedSingle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAsyncLoadEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnequipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginSkill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndSkill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShoot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExecute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeftWeaponHitted, class AProjectRCharacter*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRightWeaponHitted, class AProjectRCharacter*, Target);

UCLASS(BlueprintType)
class PROJECTR_API UWeapon final : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(const FName& InName, const struct FWeaponData& Data);

	void Equip();
	void Unequip();

	void UseSkill(uint8 Index);
	bool CanUseSkill(uint8 Index);

	UFUNCTION(BlueprintCallable)
	void RegisterOnAsyncLoadEnded(const FOnAsyncLoadEndedSingle& Callback);

	FORCEINLINE const FName& GetName() const noexcept { return Name; }
	FORCEINLINE uint8 GetKey() const noexcept { return Key; }

	FORCEINLINE class UStaticMesh* GetRightWeaponMesh() const noexcept { return RightWeaponMesh; }
	FORCEINLINE const FTransform& GetRightWeaponTransform() const noexcept { return RightWeaponTransform; }

	FORCEINLINE UStaticMesh* GetLeftWeaponMesh() const noexcept { return LeftWeaponMesh; }
	FORCEINLINE const FTransform& GetLeftWeaponTransform() const noexcept { return LeftWeaponTransform; }

private:
	void AsyncLoad(class UStaticMesh*& Ptr, const TAssetPtr<UStaticMesh>& SoftPtr);

	FORCEINLINE void CheckAndCallAsyncLoadDelegate() { if (--AsyncLoadCount == 0) OnAsyncLoadEnded.Broadcast(); }

public:
	UPROPERTY(BlueprintAssignable)
	FOnEquipped OnEquipped;

	UPROPERTY(BlueprintAssignable)
	FOnUnequipped OnUnequipped;

	UPROPERTY(BlueprintAssignable)
	FOnBeginSkill OnBeginSkill;

	UPROPERTY(BlueprintAssignable)
	FOnEndSkill OnEndSkill;

	UPROPERTY(BlueprintAssignable)
	FOnBeginAttack OnBeginAttack;

	UPROPERTY(BlueprintAssignable)
	FOnEndAttack OnEndAttack;

	UPROPERTY(BlueprintAssignable)
	FOnShoot OnShoot;

	UPROPERTY(BlueprintAssignable)
	FOnExecute OnExecute;

	UPROPERTY(BlueprintAssignable)
	FOnLeftWeaponHitted OnLeftWeaponHitted;

	UPROPERTY(BlueprintAssignable)
	FOnRightWeaponHitted OnRightWeaponHitted;

private:
	UPROPERTY()
	class AProjectRCharacter* User;

	UPROPERTY()
	TArray<class USkill*> Skills;

	UPROPERTY()
	TSubclassOf<class UAnimInstance> UpperAnimInstance;

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
