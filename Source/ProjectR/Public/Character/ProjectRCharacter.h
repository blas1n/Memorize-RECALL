// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "ProjectRCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLand, const FHitResult&, Hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttack, int32, Damage, AActor*, Target, TSubclassOf<UDamageType>, DamageType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamage, int32, Damage, AActor*, Target, TSubclassOf<UDamageType>, DamageType);

UCLASS(Abstract, Blueprintable)
class PROJECTR_API AProjectRCharacter final : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AProjectRCharacter();

	void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override;
	
	FGenericTeamId GetGenericTeamId() const;
	void SetGenericTeamId(const FGenericTeamId& NewTeamID);

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

	bool ShouldTakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) const override;

	void Landed(const FHitResult& Hit) override;

	void Initialize();

	UFUNCTION()
	void HealHealthAndEnergy(int32 Damage, AActor* Target, TSubclassOf<UDamageType> DamageType);

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
	FOnDamage OnDamage;

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
