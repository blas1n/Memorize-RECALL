// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ProjectRPlayerState.generated.h"

UCLASS(BlueprintType)
class PROJECTR_API AProjectRPlayerState final : public APlayerState
{
	GENERATED_BODY()

public:
	void InitFromDataTable(const FName& Name);

	UFUNCTION(BlueprintCallable) 
	void HealHealth(int32 Value) noexcept;

	UFUNCTION(BlueprintCallable)
	void HealHealthByDamage(int32 Damage) noexcept;

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(int32 Value, bool bWithCur = true) noexcept;

	UFUNCTION(BlueprintCallable)
	void SetHealthHeal(float Value) noexcept;

	UFUNCTION(BlueprintCallable)
	void HealEnergy(int32 Value) noexcept;

	UFUNCTION(BlueprintCallable)
	void HealEnergyByDamage(int32 Damage) noexcept;

	UFUNCTION(BlueprintCallable)
	void SetMaxEnergy(int32 Value, bool bWithCur = true) noexcept;

	UFUNCTION(BlueprintCallable)
	void SetEnergyHeal(float Value) noexcept;

	UFUNCTION(BlueprintSetter)
	void SetRunSpeed(float Value) noexcept;

	UFUNCTION(BlueprintSetter)
	void SetWalkSpeed(float Value) noexcept;

	UFUNCTION(BlueprintCallable)
	class UBuff* GetBuff(TSubclassOf<UBuff> BuffClass) const;

	FORCEINLINE int32 GetHealth() const noexcept { return Health; }
	FORCEINLINE int32 GetMaxHealth() const noexcept { return MaxHealth; }
	FORCEINLINE float GetHealthHeal() const noexcept { return HealthHeal; }

	FORCEINLINE int32 GetEnergy() const noexcept { return Energy; }
	FORCEINLINE int32 GetMaxEnergy() const noexcept { return MaxEnergy; }
	FORCEINLINE float GetEnergyHeal() const noexcept { return EnergyHeal; }

	FORCEINLINE float GetRunSpeed() const noexcept { return RunSpeed; }
	FORCEINLINE float GetWalkSpeed() const noexcept { return WalkSpeed; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = true))
	int32 Health;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = true))
	int32 MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = true))
	float HealthHeal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = true))
	int32 Energy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = true))
	int32 MaxEnergy;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = true))
	float EnergyHeal;

	UPROPERTY(EditAnywhere, BlueprintSetter = SetRunSpeed, Category = Speed, meta = (AllowPrivateAccess = true))
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintSetter = SetWalkSpeed, Category = Speed, meta = (AllowPrivateAccess = true))
	float WalkSpeed;

	UPROPERTY()
	mutable TArray<UBuff*> Buffs;
};
