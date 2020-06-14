// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectRCharacter.h"
#include "PlayerCharacter.generated.h"

UCLASS(Abstract, Blueprintable)
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
	void Tick(float DeltaTimes) override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void CreateWeapons(TArray<FName>&& WeaponNames) override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void AddYawInput(float Value);
	void AddPitchInput(float Value);

	void ToggleRun();

	void PressDodge();
	void ReleaseDodge();

	void SwapWeapon(uint8 Index);
	void SwapWeapon(float Value);

	// Need Optimization
	void LockOn();
	void LockOff();

	bool CheckLockOn(const AActor* Enemy, float& OutAngle, float& OutDistance) const;

	UFUNCTION()
	void HealEnergyByAttack(AProjectRCharacter* Target, int32 Damage);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, EditFixedSize, Category = Weapon, meta = (AllowPrivateAccess = true))
	TArray<class AWeapon*> Weapons;

	UPROPERTY()
	AWeapon* Unequip;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	int32 Energy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	int32 MaxEnergy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	float EnergyHeal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	float RunSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LockOn, meta = (AllowPrivateAccess = true))
	TSubclassOf<AProjectRCharacter> EnemyClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LockOn, meta = (AllowPrivateAccess = true))
	float LockOnDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LockOn, meta = (AllowPrivateAccess = true))
	float LoseLockOnDistance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = LockOn, meta = (AllowPrivateAccess = true))
	float LockOnAngle;

	UPROPERTY()
	AProjectRCharacter* LockOnEnemy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	float JumpDelay;

	FTimerHandle DodgeTimer;
	uint8 CurWeaponIndex;
	uint8 bIsReadyDodge : 1;
	uint8 bIsRun : 1;
};
