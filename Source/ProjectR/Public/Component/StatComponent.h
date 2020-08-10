// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangedLevel, uint8);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTR_API UStatComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatComponent();

	UFUNCTION(BlueprintSetter)
	void SetLevel(uint8 NewLevel);

	UFUNCTION(BlueprintCallable)
	void Heal(float Value);

	UFUNCTION(BlueprintCallable)
	void HealByDamage(float Damage);

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float Value, bool bWithCur = true);

	UFUNCTION(BlueprintCallable)
	void SetHealValue(float Value);

	UFUNCTION(BlueprintSetter)
	void SetRunSpeed(float Value);

	UFUNCTION(BlueprintSetter)
	void SetWalkSpeed(float Value);

	UFUNCTION(BlueprintSetter)
	void SetLockSpeed(float Value);

	class UBuff* GetBuff(TSubclassOf<UBuff> BuffClass) const;

	FORCEINLINE uint8 GetLevel() const noexcept { return Level; }

	FORCEINLINE float GetHealth() const noexcept { return Health; }
	FORCEINLINE float GetMaxHealth() const noexcept { return MaxHealth; }
	FORCEINLINE float GetHealthHeal() const noexcept { return HealthHeal; }

	FORCEINLINE float GetRunSpeed() const noexcept { return RunSpeed; }
	FORCEINLINE float GetWalkSpeed() const noexcept { return WalkSpeed; }
	FORCEINLINE float GetLockSpeed() const noexcept { return LockSpeed; }

private:
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

	void InitData();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastHeal(float Value);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetMaxHealth(float Value, bool bWithCur);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetHealValue(float Value);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetRunSpeed(float Value);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetWalkSpeed(float Value);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetLockSpeed(float Value);

	void MulticastHeal_Implementation(float Value);
	void MulticastSetMaxHealth_Implementation(float Value, bool bWithCur);
	void MulticastSetHealValue_Implementation(float Value);

	void MulticastSetRunSpeed_Implementation(float Value);
	void MulticastSetWalkSpeed_Implementation(float Value);
	void MulticastSetLockSpeed_Implementation(float Value);

	UFUNCTION()
	void OnLockApplied();

	UFUNCTION()
	void OnLockReleased();

	UFUNCTION()
	void OnRunApplied();

	UFUNCTION()
	void OnRunReleased();

	void SetMovement();

public:
	FOnChangedLevel OnChangedLevel;

private:
	UPROPERTY(EditAnywhere, Category = Data, meta = (AllowPrivateAccess = true))
	uint8 StatKey;

	UPROPERTY(EditAnywhere, BlueprintSetter = SetLevel, Category = Data, meta = (AllowPrivateAccess = true))
	uint8 Level;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = true))
	float Health;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = true))
	float MaxHealth;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = true))
	float HealthHeal;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintSetter = SetRunSpeed, Category = Speed, meta = (AllowPrivateAccess = true))
	float RunSpeed;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintSetter = SetWalkSpeed, Category = Speed, meta = (AllowPrivateAccess = true))
	float WalkSpeed;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintSetter = SetLockSpeed, Category = Speed, meta = (AllowPrivateAccess = true))
	float LockSpeed;

	UPROPERTY()
	mutable TArray<UBuff*> Buffs;

	UPROPERTY()
	class UDataTable* StatDataTable;

	uint8 bIsRunned : 1;
	uint8 bIsLocked : 1;
};
