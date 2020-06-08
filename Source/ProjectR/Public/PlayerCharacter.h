// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectRCharacter.h"
#include "PlayerCharacter.generated.h"

UCLASS(BlueprintType)
class PROJECTR_API APlayerCharacter final : public AProjectRCharacter
{
	GENERATED_BODY()
	
public:
	APlayerCharacter();

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(FName Name, uint8 Index);

	UFUNCTION(BlueprintCallable)
	int32 HealEnergy(int32 Value);

	UFUNCTION(BlueprintCallable)
	int32 SetMaxEnergy(int32 NewMaxEnergy);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const noexcept { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const noexcept { return FollowCamera; }

	FORCEINLINE int32 GetEnergy() const noexcept { return Energy; }
	FORCEINLINE int32 GetMaxEnergy() const noexcept { return MaxEnergy; }
	FORCEINLINE int32 GetEnergyHeal() const noexcept { return EnergyHeal; }

private:
	void BeginPlay() override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void HealEnergyByAttack(AProjectRCharacter* Target, int32 Damage);

	void MoveForward(float Value);
	void MoveRight(float Value);

	void SwapWeapon(uint8 Index);
	void SwapWeapon(float Value);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, EditFixedSize, Category = Weapon, meta = (AllowPrivateAccess = true))
	TArray<class AWeapon*> Weapons;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	int32 Energy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	int32 MaxEnergy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	float EnergyHeal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, Category = Combat, meta = (AllowPrivateAccess = true))
	float DisableCombatDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, Category = Combat, meta = (AllowPrivateAccess = true))
	int32 EnergyPerTick;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, Category = Combat, meta = (AllowPrivateAccess = true))
	float EnergyTick;

	FTimerHandle EnergyTimer;
	uint8 CurWeaponIndex;
};
