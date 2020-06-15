// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ProjectRPlayerState.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FStat
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void HealStat(uint8 Value) noexcept;

	UFUNCTION(BlueprintCallable)
	void HealStatByDamage(uint8 Damage) noexcept;

	UFUNCTION(BlueprintCallable)
	void SetMaxStat(uint8 Value, bool bWithCur = true) noexcept;

	UFUNCTION(BlueprintCallable)
	void SetStatHeal(float Value) noexcept;

	FORCEINLINE uint8 GetStat() const noexcept { return CurStat; }
	FORCEINLINE uint8 GetMaxStat() const noexcept { return MaxStat; }
	FORCEINLINE float GetStatHeal() const noexcept { return StatHeal; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = true))
	uint8 CurStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = true))
	uint8 MaxStat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat, meta = (AllowPrivateAccess = true))
	float StatHeal;
};

UCLASS()
class PROJECTR_API AProjectRPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	FORCEINLINE FStat& GetHealth() noexcept { return Health; }
	FORCEINLINE const FStat& GetHealth() const noexcept { return Health; }

	FORCEINLINE FStat& GetEnergy() noexcept { return Energy; }
	FORCEINLINE const FStat& GetEnergy() const noexcept { return Energy; }

	FORCEINLINE void SetRunSpeed(float Value) noexcept { RunSpeed = Value; }
	FORCEINLINE void SetWalkSpeed(float Value) noexcept { WalkSpeed = Value; }

	UFUNCTION(BlueprintSetter)
	void SetCrouchSpeed(float Value) noexcept;

	FORCEINLINE float GetRunSpeed() const noexcept { return RunSpeed; }
	FORCEINLINE float GetWalkSpeed() const noexcept { return WalkSpeed; }
	FORCEINLINE float GetCrouchSpeed() const noexcept { return CrouchSpeed; }

	FORCEINLINE TMap<TSubclassOf<class UBuff>, class UBuffStorage*>& GetBuffStorages() noexcept { return BuffStorages; }
	FORCEINLINE const TMap<TSubclassOf<UBuff>, UBuffStorage*>& GetBuffStorages() const noexcept { return BuffStorages; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Stat, meta = (AllowPrivateAccess = true))
	FStat Health;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Stat, meta = (AllowPrivateAccess = true))
	FStat Energy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed, meta = (AllowPrivateAccess = true))
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Speed, meta = (AllowPrivateAccess = true))
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintSetter = SetCrouchSpeed, Category = Speed, meta = (AllowPrivateAccess = true))
	float CrouchSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Buff, meta = (AllowPrivateAccess = true))
	TMap<TSubclassOf<UBuff>, UBuffStorage*> BuffStorages;
};
