// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectRCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AController*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttack, AProjectRCharacter*, Target);

UCLASS(config=Game, BlueprintType)
class AProjectRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AProjectRCharacter();

	class AWeapon* GenerateWeapon(const FName& Name);
	int32 HealHealth(int32 Value);

	float GetSpeed() const noexcept;
	void SetSpeed(float Speed) noexcept;

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const noexcept { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const noexcept { return FollowCamera; }
	FORCEINLINE class UStaticMeshComponent* GetLeftWeapon() const noexcept { return LeftWeapon; }
	FORCEINLINE UStaticMeshComponent* GetRightWeapon() const noexcept { return RightWeapon; }

	FORCEINLINE class AWeapon* GetWeapon() const noexcept { return Weapon; }
	FORCEINLINE int32 GetHealth() const noexcept { return Health; }
	FORCEINLINE int32 GetMaxHealth() const noexcept { return MaxHealth; }
	FORCEINLINE float GetHealthHeal() const noexcept { return HealthHeal; }
	FORCEINLINE float GetWalkSpeed() const noexcept { return WalkSpeed; }
	FORCEINLINE float GetRunSpeed() const noexcept { return RunSpeed; }

protected:
	void BeginPlay() override;
	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	void PressSkill(uint8 index);
	void ReleaseSkill(uint8 index);

	FORCEINLINE void SetWeapon(AWeapon* InWeapon) noexcept { Weapon = InWeapon; }

private:
	void Death();

public:
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
	FOnAttack OnAttack;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = true))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* LeftWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* RightWeapon;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Transient, Category = Weapon, meta = (AllowPrivateAccess = true))
	AWeapon* Weapon;

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

	uint8 StunCount;
};
