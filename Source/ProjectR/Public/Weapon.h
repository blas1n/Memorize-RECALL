// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnAsyncLoadEndedSingle);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAsyncLoadEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInactive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginSkill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndSkill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShoot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExecute);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeftWeaponHitted, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRightWeaponHitted, AActor*, Target);

USTRUCT(Atomic, BlueprintType)
struct PROJECTR_API FWeaponInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Transform;
};

UCLASS(BlueprintType)
class PROJECTR_API AWeapon final : public AActor
{
	GENERATED_BODY()

public:
	AWeapon();

	void Initialize(const struct FWeaponData* WeaponData);

	UFUNCTION(BlueprintCallable)
	void Equip();

	UFUNCTION(BlueprintCallable)
	void Unequip();

	UFUNCTION(BlueprintCallable)
	void UseSkill(uint8 Index);

	UFUNCTION(BlueprintCallable)
	void RegisterOnAsyncLoadEnded(const FOnAsyncLoadEndedSingle& Callback);

private:
	void BeginPlay() override;

	void EquipOnce(UStaticMeshComponent* Weapon, const FWeaponInfo& Info);
	void UnequipOnce(UStaticMeshComponent* Weapon);

	void LoadWeapon(FWeaponInfo& WeaponInfo, const TAssetPtr<UStaticMesh>& Mesh, const FTransform& Transform);
	void OnMeshLoaded(FWeaponInfo& WeaponInfo, const TAssetPtr<UStaticMesh>& MeshPtr);

	void OnEquipMontageLoaded(const TAssetPtr<UAnimMontage>& MontagePtr);

	UFUNCTION()
	void BeginSkill(UAnimMontage* Montage);

	UFUNCTION()
	void EndSkill(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnLeftWeaponOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRightWeaponOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(BlueprintAssignable)
	FOnActive OnActive;

	UPROPERTY(BlueprintAssignable)
	FOnInactive OnInactive;

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* LeftWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* RightWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	FWeaponInfo LeftWeaponInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	FWeaponInfo RightWeaponInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Info, meta = (AllowPrivateAccess = true))
	int32 Key;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Info, meta = (AllowPrivateAccess = true))
	FName Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Skill, meta = (AllowPrivateAccess = true))
	TArray<class ASkill*> Skills;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	UAnimMontage* EquipMontage;

	FOnAsyncLoadEnded OnAsyncLoadEnded;

	uint8 AsyncLoadCount;
};