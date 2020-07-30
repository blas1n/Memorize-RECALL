// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponOverlapped, AActor*, Target);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTR_API UWeaponComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponComponent();

	void Initialize(const TArray<int32>& Keies);

	UFUNCTION(BlueprintCallable)
	void StartSkill(uint8 Index);

	UFUNCTION(BlueprintCallable)
	void EndSkill(uint8 Index);

	UFUNCTION(BlueprintCallable)
	bool CanUseSkill(uint8 Index) const;

	UFUNCTION(BlueprintCallable)
	void SwapWeapon(uint8 Index);

	UFUNCTION(BlueprintCallable)
	void CreateNewWeapon(int32 Key, uint8 Index);

	void SetWeaponCollision(bool bEnableRight, bool bEnableLeft);

	UFUNCTION(BlueprintCallable)
	class UWeapon* GetWeapon() noexcept { return Weapons[CurIndex]; }
	
	FORCEINLINE const UWeapon* GetWeapon() const noexcept { return Weapons[CurIndex]; }

	FORCEINLINE class UStaticMeshComponent* GetRightWeapon() const noexcept { return RightWeapon; }
	FORCEINLINE UStaticMeshComponent* GetLeftWeapon() const noexcept { return LeftWeapon; }	

	FORCEINLINE uint8 GetWeaponNum() const noexcept { return WeaponNum; }
	FORCEINLINE uint8 GetWeaponIndex() const noexcept { return CurIndex; }

private:
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason);

	UStaticMeshComponent* CreateWeaponComponent(const FName& Name, const FName& SocketName);
	void EquipWeapon(UWeapon* NewWeapon, bool bNeedUnequip);

	UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Detach();

	void DetachOnce(class UStaticMeshComponent* Weapon);

public:
	UPROPERTY(BlueprintAssignable)
	FOnWeaponOverlapped OnWeaponOverlapped;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* RightWeapon;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* LeftWeapon;

	UPROPERTY()
	TArray<UWeapon*> Weapons;

	UPROPERTY()
	class AProjectRCharacter* User;

	uint8 WeaponNum;
	uint8 CurIndex;
};
