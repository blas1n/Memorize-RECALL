// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ProjectRPlayerState.generated.h"

UCLASS()
class PROJECTR_API AProjectRPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	void HealHealth(uint8 Value) noexcept;
	void SetMaxHealth(uint8 Value, bool bWithHealth = true) noexcept;
	FORCEINLINE void SetHealthHeal(float Value) noexcept { HealthHeal = Value; }

	void HealEnergy(uint8 Value) noexcept;
	void SetMaxEnergy(uint8 Value, bool bWithHealth = true) noexcept;
	FORCEINLINE void SetEnergyHeal(float Value) noexcept { EnergyHeal = Value; }

	FORCEINLINE void SetRunSpeed(float Value) noexcept { RunSpeed = Value; }
	FORCEINLINE void SetWalkSpeed(float Value) noexcept { WalkSpeed = Value; }
	FORCEINLINE void SetCrouchSpeed(float Value) noexcept { CrouchSpeed = Value; }

	FORCEINLINE uint8 GetHealth() const noexcept { return Health; }
	FORCEINLINE uint8 GetMaxHealth() const noexcept { return MaxHealth; }
	FORCEINLINE float GetHealthHeal() const noexcept { return HealthHeal; }

	FORCEINLINE uint8 GetEnergy() const noexcept { return Energy; }
	FORCEINLINE uint8 GetMaxEnergy() const noexcept { return MaxEnergy; }
	FORCEINLINE float GetEnergyHeal() const noexcept { return EnergyHeal; }

	FORCEINLINE float GetRunSpeed() const noexcept { return RunSpeed; }
	FORCEINLINE float GetWalkSpeed() const noexcept { return WalkSpeed; }
	FORCEINLINE float GetCrouchSpeed() const noexcept { return CrouchSpeed; }

	FORCEINLINE TMap<TSubclassOf<class UBuff>, class UBuffStorage*>& GetBuffStorages() noexcept { return BuffStorages; }
	FORCEINLINE const TMap<TSubclassOf<UBuff>, UBuffStorage*>& GetBuffStorages() const noexcept { return BuffStorages; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, meta = (AllowPrivateAccess = true))
	uint8 Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, meta = (AllowPrivateAccess = true))
	uint8 MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, meta = (AllowPrivateAccess = true))
	float HealthHeal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, meta = (AllowPrivateAccess = true))
	uint8 Energy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, meta = (AllowPrivateAccess = true))
	uint8 MaxEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat, meta = (AllowPrivateAccess = true))
	float EnergyHeal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed, meta = (AllowPrivateAccess = true))
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed, meta = (AllowPrivateAccess = true))
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed, meta = (AllowPrivateAccess = true))
	float CrouchSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Buff, meta = (AllowPrivateAccess = true))
	TMap<TSubclassOf<UBuff>, UBuffStorage*> BuffStorages;
};
