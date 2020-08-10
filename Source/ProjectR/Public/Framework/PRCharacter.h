// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Interface/ComponentOwner.h"
#include "PRCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLand, const FHitResult&, Hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttack, float, Damage, AActor*, Target, TSubclassOf<UDamageType>, DamageType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamage, float, Damage, AActor*, Target, TSubclassOf<UDamageType>, DamageType);

UCLASS(BlueprintType)
class PROJECTR_API APRCharacter final : public ACharacter, public IGenericTeamAgentInterface, public IComponentOwner
{
	GENERATED_BODY()

public:
	APRCharacter();

	FORCEINLINE void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override
	{
		GetLookLocationAndRotation(Location, Rotation);
	}
	
	FORCEINLINE FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId{ TeamId }; }
	FORCEINLINE void SetGenericTeamId(const FGenericTeamId& NewTeamId) override { TeamId = NewTeamId.GetId(); }
	FORCEINLINE void SetGenericTeamId(uint8 NewTeamId) { TeamId = NewTeamId; }
	FORCEINLINE bool IsDeath() const noexcept { return bIsDeath; }

protected:
	UFUNCTION(BlueprintNativeEvent)
	void GetLookLocationAndRotation(FVector& Location, FRotator& Rotation) const;

	FORCEINLINE UWeaponComponent* GetWeaponComponent_Implementation() const override { return WeaponComponent; }
	FORCEINLINE UStatComponent* GetStatComponent_Implementation() const override { return StatComponent; }

private:
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void PostInitializeComponents() override;
	void BeginPlay() override;

	float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	bool ShouldTakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) const override;

	void Landed(const FHitResult& Hit) override;

	void Initialize();

	UFUNCTION()
	void Heal(float Damage, AActor* Target, TSubclassOf<UDamageType> DamageType);

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStatComponent* StatComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	uint8 CharacterKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, meta = (AllowPrivateAccess = true))
	uint8 TeamId;

	UPROPERTY()
	class UDataTable* CharacterDataTable;

	uint8 bIsDeath : 1;
};
