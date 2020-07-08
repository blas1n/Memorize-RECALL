// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/AssetManager.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnAsyncLoadEndedSingle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAsyncLoadEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnequipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShoot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExecute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBeginSkill, class USkill*, Skill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndSkill, USkill*, Skill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponHitted, class AProjectRCharacter*, Target);

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
	void SetWeaponCollision(bool bRightWeaponEnable, bool bLeftWeaponEnable);

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

	void AddComponents(class USkill* Skill);

	template <class T>
	void AsyncLoad(T*& Ptr, const TAssetPtr<T>& SoftPtr)
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

	FORCEINLINE void CheckAndCallAsyncLoadDelegate() { if (--AsyncLoadCount == 0) OnAsyncLoadEnded.Broadcast(); }

public:
	UPROPERTY(BlueprintAssignable)
	FOnEquipped OnEquipped;

	UPROPERTY(BlueprintAssignable)
	FOnUnequipped OnUnequipped;

	UPROPERTY(BlueprintAssignable)
	FOnBeginAttack OnBeginAttack;

	UPROPERTY(BlueprintAssignable)
	FOnEndAttack OnEndAttack;

	UPROPERTY(BlueprintAssignable)
	FOnShoot OnShoot;

	UPROPERTY(BlueprintAssignable)
	FOnExecute OnExecute;

	UPROPERTY(BlueprintAssignable)
	FOnBeginSkill OnBeginSkill;

	UPROPERTY(BlueprintAssignable)
	FOnEndSkill OnEndSkill;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponHitted OnWeaponHitted;

private:
	UPROPERTY()
	class AProjectRCharacter* User;

	UPROPERTY()
	TArray<class USkill*> Skills;
	UPROPERTY()
	TMap<FName, UActorComponent*> Components;

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
	uint8 UseSkillCount;
	uint8 AsyncLoadCount;
};
