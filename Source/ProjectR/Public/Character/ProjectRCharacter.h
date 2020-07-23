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

	void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;

	FORCEINLINE class UWeaponComponent* GetWeaponComponent() const noexcept { return WeaponComponent; }
	FORCEINLINE int32 GetKey() const noexcept { return Key; }
	FORCEINLINE int32 GetLevel() const noexcept { return Level; }
	FORCEINLINE bool IsDeath() const noexcept { return bIsDeath; }

protected:
	UFUNCTION(BlueprintNativeEvent)
	void GetLookLocationAndRotation(FVector& Location, FRotator& Rotation) const;

private:
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void PostInitializeComponents() override;
	
	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	void Landed(const FHitResult& Hit) override;

	void Initialize();

	UFUNCTION()
	void HealHealthAndEnergy(AProjectRCharacter* Target, int32 Damage);

	void GetLookLocationAndRotation_Implementation(FVector& Location, FRotator& Rotation) const;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	int32 Key;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	TArray<int32> WeaponKeies;

	UPROPERTY()
	class UDataTable* CharacterDataTable;

	uint8 bIsDeath : 1;
};
