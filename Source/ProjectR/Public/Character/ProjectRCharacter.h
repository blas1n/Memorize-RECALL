// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectRCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeath, AController*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLand, const FHitResult&, Hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttack, AProjectRCharacter*, Target, int32, Damage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamaged, AController*, Instigator, int32, Damage);

UCLASS(Abstract, Blueprintable)
class PROJECTR_API AProjectRCharacter final : public ACharacter
{
	GENERATED_BODY()

public:
	AProjectRCharacter();

	UFUNCTION(BlueprintCallable)
	void Attack(AProjectRCharacter* Target, int32 Damage);

	UFUNCTION(BlueprintNativeEvent)
	FVector GetViewLocation() const;
	
	UFUNCTION(BlueprintImplementableEvent)
	TArray<FName> GetWeaponNames();

	FORCEINLINE class UWeaponComponent* GetWeaponComponent() const noexcept { return WeaponComponent; }
	FORCEINLINE const FName& GetName() const noexcept { return Name; }
	FORCEINLINE bool IsDeath() const noexcept { return bIsDeath; }

private:
	void BeginPlay() override;

	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	void Landed(const FHitResult& Hit) override;

	UFUNCTION()
	void HealHealthAndEnergy(AProjectRCharacter* Target, int32 Damage);

	FVector GetViewLocation_Implementation() const;
	void Death();

public:
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
	FOnLand OnLand;

	UPROPERTY(BlueprintAssignable)
	FOnAttack OnAttack;

	UPROPERTY(BlueprintAssignable)
	FOnDamaged OnDamaged;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UWeaponComponent* WeaponComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FName Name;

	uint8 bIsDeath : 1;
};
