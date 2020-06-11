// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectRCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamaged, AController*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AController*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipped, class AWeapon*, Weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttack, AProjectRCharacter*, Target, int32, Damage);

UCLASS(config=Game, Abstract, Blueprintable)
class AProjectRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AProjectRCharacter();

	UFUNCTION(BlueprintCallable)
	void Attack(AProjectRCharacter* Target, int32 Damage, AActor* Causer);

	UFUNCTION(BlueprintCallable)
	void BeginParrying(UObject* InParrying);

	UFUNCTION(BlueprintCallable)
	void EndParrying(UObject* InParrying);

	UFUNCTION(BlueprintCallable)
	void ApplyStun();

	UFUNCTION(BlueprintCallable)
	void ReleaseStun();

	UFUNCTION(BlueprintCallable)
	int32 HealHealth(int32 Value);

	UFUNCTION(BlueprintCallable)
	int32 SetMaxHealth(int32 NewMaxHealth);

	UFUNCTION(BlueprintCallable)
	float GetSpeed() const noexcept;

	UFUNCTION(BlueprintCallable)
	void SetSpeed(float Speed) noexcept;

	FORCEINLINE class UStaticMeshComponent* GetLeftWeapon() const noexcept { return LeftWeapon; }
	FORCEINLINE UStaticMeshComponent* GetRightWeapon() const noexcept { return RightWeapon; }

	FORCEINLINE class AWeapon* GetWeapon() const noexcept { return Weapon; }
	FORCEINLINE int32 GetHealth() const noexcept { return Health; }
	FORCEINLINE int32 GetMaxHealth() const noexcept { return MaxHealth; }
	FORCEINLINE float GetHealthHeal() const noexcept { return HealthHeal; }
	FORCEINLINE float GetWalkSpeed() const noexcept { return WalkSpeed; }
	FORCEINLINE float GetRunSpeed() const noexcept { return RunSpeed; }
	FORCEINLINE TMap<TSubclassOf<class ABuff>, class UBuffStorage*>&
		GetBuffStorages() noexcept { return BuffStorages; }

	FORCEINLINE bool IsCasting() const noexcept { return bIsCasting; }
	FORCEINLINE bool CanMoving() const noexcept { return bCanMoving; }

	FORCEINLINE void SetIsCasting(bool bInIsCasting) noexcept { bIsCasting = bInIsCasting; }
	FORCEINLINE void SetCanMoving(bool bInCanMoving) noexcept { bCanMoving = bInCanMoving; }

protected:
	void BeginPlay() override;
	
	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	class AWeapon* GenerateWeapon(FName Name);

	UFUNCTION(BlueprintCallable)
	void SetWeapon(AWeapon* InWeapon);

	UFUNCTION(BlueprintImplementableEvent)
	void OnStunApply();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStunRelease();

	virtual void NativeOnStunApply() {}
	virtual void NativeOnStunRelease() {}

	void UseSkill(uint8 index);

	FORCEINLINE AWeapon* GetUnequip() const noexcept { return Weapon; }

private:
	void Death();

	UFUNCTION()
	void OnAttacked(AProjectRCharacter* Target, int32 Damage);

	UFUNCTION()
	void Equip();

public:
	UPROPERTY(BlueprintAssignable)
	FOnDamaged OnDamaged;

	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
	FOnEquipped OnEquipped;

	UPROPERTY(BlueprintAssignable)
	FOnAttack OnAttack;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* LeftWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* RightWeapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = Weapon, meta = (AllowPrivateAccess = true))
	AWeapon* Weapon;

	UPROPERTY()
	AWeapon* Unequip;

	UPROPERTY(EditDefaultsOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	class UDataTable* WeaponsData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	int32 Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	int32 MaxHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	float HealthHeal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, Category = Stat, meta = (AllowPrivateAccess = true))
	float RunSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Buff, meta = (AllowPrivateAccess = true))
	TMap<TSubclassOf<class ABuff>, class UBuffStorage*> BuffStorages;

	UObject* Parrying;

	bool bIsCasting;
	bool bCanMoving;
};
