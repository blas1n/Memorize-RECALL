// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Data/MoveState.h"
#include "PRCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLand, const FHitResult&, Hit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAttack, float, Damage, AActor*, Target, TSubclassOf<UDamageType>, DamageType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamage, float, Damage, AActor*, Target, TSubclassOf<UDamageType>, DamageType);

UCLASS(BlueprintType)
class PROJECTR_API APRCharacter final : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	APRCharacter();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetParryingObject(UObject* NewParryingObject);

	UFUNCTION(BlueprintCallable)
	void SetMoveState(EMoveState NewMoveState);

	FORCEINLINE void GetActorEyesViewPoint(FVector& Location, FRotator& Rotation) const override
	{
		GetLookLocationAndRotation(Location, Rotation);
	}
	
	FORCEINLINE FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId{ TeamId }; }
	FORCEINLINE void SetGenericTeamId(const FGenericTeamId& NewTeamId) override { TeamId = NewTeamId.GetId(); }
	FORCEINLINE void SetGenericTeamId(uint8 NewTeamId) { TeamId = NewTeamId; }
	
	FORCEINLINE class UWeaponComponent* GetWeaponComponent() const noexcept { return WeaponComponent; }
	FORCEINLINE class UStatComponent* GetStatComponent() const noexcept { return StatComponent; }

	FORCEINLINE EMoveState GetMoveState() const noexcept { return MoveState; }
	FORCEINLINE AActor* GetLockTarget() const noexcept { return LockTarget; }
	FORCEINLINE bool IsLocked() const noexcept { return bIsLocked; }
	FORCEINLINE bool IsDeath() const noexcept { return bIsDeath; }

protected:
	UFUNCTION(BlueprintNativeEvent)
	void GetLookLocationAndRotation(FVector& Location, FRotator& Rotation) const;

private:
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void PostInitializeComponents() override;
	void Tick(float DeltaSeconds) override;

	float TakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	bool ShouldTakeDamage(float Damage, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) const override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Landed(const FHitResult& Hit) override;

	void Initialize();
	void Death();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetMoveState(EMoveState NewMoveState);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerLock(AActor* NewLockTarget);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUnlock();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDeath();

	void ServerSetMoveState_Implementation(EMoveState NewMoveState);
	FORCEINLINE bool ServerSetMoveState_Validate(EMoveState NewMoveState) const noexcept { return true; }

	void ServerLock_Implementation(AActor* NewLockTarget);
	FORCEINLINE bool ServerLock_Validate(AActor* NewLockTarget) const noexcept { return true; }

	void ServerUnlock_Implementation();
	FORCEINLINE bool ServerUnlock_Validate() const noexcept { return true; }

	void MulticastDeath_Implementation();

	UFUNCTION()
	void OnRep_MoveState();

	UFUNCTION()
	void OnRep_LockTarget();

	UFUNCTION()
	void OnRep_IsLocked();

	void SetMovement();

	void GetLookLocationAndRotation_Implementation(FVector& Location, FRotator& Rotation) const;

	bool IsParryable(float Damage, APRCharacter* Causer);
	
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

	UPROPERTY(ReplicatedUsing = OnRep_MoveState)
	EMoveState MoveState;

	UPROPERTY(ReplicatedUsing = OnRep_LockTarget)
	AActor* LockTarget;

	UPROPERTY(ReplicatedUsing = OnRep_IsLocked)
	uint8 bIsLocked : 1;

	UPROPERTY(Transient)
	UObject* ParryingObject;

	UPROPERTY()
	class UDataTable* CharacterDataTable;

	uint8 bIsDeath : 1;
};
