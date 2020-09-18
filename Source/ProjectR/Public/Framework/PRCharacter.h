// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "PRCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocked, bool, bIsLock);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamaged, float, Damage, APRCharacter*, Causer);

UCLASS(BlueprintType)
class PROJECTR_API APRCharacter final : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	APRCharacter(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Heal(float Value);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetMaxHealth(float NewMaxHealth, bool bWithCurrent);

	UFUNCTION(BlueprintCallable)
	void Lock(AActor* NewLockTarget);

	UFUNCTION(BlueprintCallable)
	void Unlock();
	
	FORCEINLINE FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId{ TeamId }; }
	FORCEINLINE void SetGenericTeamId(const FGenericTeamId& NewTeamId) override { TeamId = NewTeamId.GetId(); }
	FORCEINLINE void SetGenericTeamId(uint8 NewTeamId) { TeamId = NewTeamId; }
	
	FORCEINLINE class UWeaponComponent* GetWeaponComponent() const noexcept { return WeaponComp; }
	FORCEINLINE AActor* GetLockedTarget() const noexcept { return LockedTarget; }
	FORCEINLINE bool IsLocked() const noexcept { return bIsLocked; }
	FORCEINLINE bool IsDeath() const noexcept { return bIsDeath; }

protected:
	UFUNCTION(BlueprintImplementableEvent)
	TArray<UPrimitiveComponent*> GetAttackComponents() const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_Health();

private:
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void PostInitializeComponents() override;

	void BeginPlay() override;
	
	void Tick(float DeltaSeconds) override;

	float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	bool ShouldTakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) const override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Initialize();
	void Death();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerLock(AActor* NewLockTarget);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUnlock();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnHit(float Damage, APRCharacter* Causer);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDeath();

	void ServerLock_Implementation(AActor* NewLockTarget);
	FORCEINLINE bool ServerLock_Validate(AActor* NewLockTarget) const noexcept { return true; }

	void ServerUnlock_Implementation();
	FORCEINLINE bool ServerUnlock_Validate() const noexcept { return true; }

	void MulticastOnHit_Implementation(float Damage, APRCharacter* Causer);
	void MulticastDeath_Implementation();

	UFUNCTION()
	void OnRep_IsLocked();

	void ApplyCharacterData(const struct FCharacterData& Data);
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	UPROPERTY(BlueprintAssignable)
	FOnLocked OnLocked;

	UPROPERTY(BlueprintAssignable)
	FOnDamaged OnDamaged;

private:
	UPROPERTY()
	class UDataTable* CharacterDataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UWeaponComponent* WeaponComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UWeaponMeshComponent* RightWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UWeaponMeshComponent* LeftWeapon;

	UPROPERTY(Transient, Replicated, BlueprintReadOnly, Category = Lock, meta = (AllowPrivateAccess = true))
	AActor* LockedTarget;

	UPROPERTY(ReplicatedUsing = OnRep_Health, Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float Health;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	uint8 CharacterKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data, meta = (AllowPrivateAccess = true))
	uint8 TeamId;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_IsLocked, BlueprintReadOnly, Category = Lock, meta = (AllowPrivateAccess = true))
	uint8 bIsLocked : 1;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	uint8 bIsDeath : 1;
};
