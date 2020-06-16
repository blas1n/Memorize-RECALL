// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectRCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AController*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttack, AProjectRCharacter*, Target, uint8, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamaged, AController*, Instigator, uint8 , Damage);

UCLASS(Abstract, Blueprintable)
class PROJECTR_API AProjectRCharacter final : public ACharacter
{
	GENERATED_BODY()

public:
	AProjectRCharacter();

	UFUNCTION(BlueprintCallable)
	void Attack(AProjectRCharacter* Target, uint8 Damage, AActor* Causer);

	UFUNCTION(BlueprintCallable)
	void BeginParrying(UObject* InParrying);

	UFUNCTION(BlueprintCallable)
	void EndParrying(UObject* InParrying);

	UFUNCTION(BlueprintCallable)
	void SetLockTarget(AProjectRCharacter* Target);

	UFUNCTION(BlueprintCallable)
	void Run();

	UFUNCTION(BlueprintCallable)
	void Walk();

	FORCEINLINE class UWeaponComponent* GetWeaponComponent() const noexcept { return WeaponComponent; }
	FORCEINLINE AProjectRCharacter* GetLockedTarget() const noexcept { return LockedTarget; }

	FORCEINLINE bool IsCasting() const noexcept { return bIsCasting; }
	FORCEINLINE void SetCast(bool bIsCast) noexcept { bIsCasting = bIsCast; }

	FORCEINLINE bool CanMoving() const noexcept { return bCanMoving; }
	FORCEINLINE void SetMove(bool bCanMove) noexcept { bCanMoving = bCanMove; }

	FORCEINLINE bool IsRunning() const noexcept { return bIsRunning; }
	FORCEINLINE bool IsDeath() const noexcept { return bIsDeath; }

protected:
	UFUNCTION(BlueprintImplementableEvent)
	TArray<FName> GetWeaponNames();

	UFUNCTION(BlueprintImplementableEvent)
	void OnLockedOn(AProjectRCharacter* Locker);

	UFUNCTION(BlueprintImplementableEvent)
	void OnLockedOff(AProjectRCharacter* Locker);

	UFUNCTION(BlueprintImplementableEvent)
	FVector GetViewLocation() const;

private:
	void BeginPlay() override;

	void Tick(float DeltaSeconds) override;

	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void HealHealthAndEnergy(AProjectRCharacter* Target, uint8 Damage);

	void SetControlRotationIfLocked(float DeltaSeconds);

	void Death();

public:
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
	FOnAttack OnAttack;

	UPROPERTY(BlueprintAssignable)
	FOnDamaged OnDamaged;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UWeaponComponent* WeaponComponent;

	UPROPERTY(EditAnywhere, Category = Data, meta = (AllowPrivateAccess = true))
	FName StatDataRowName;

	UPROPERTY()
	UObject* Parrying;

	UPROPERTY()
	AProjectRCharacter* LockedTarget;

	uint8 bIsCasting : 1;
	uint8 bCanMoving : 1;
	uint8 bIsRunning : 1;
	uint8 bIsDeath : 1;
};
