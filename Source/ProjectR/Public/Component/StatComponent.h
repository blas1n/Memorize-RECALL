// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangedLevel, uint8);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTR_API UStatComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatComponent();

	UFUNCTION(BlueprintSetter, BlueprintAuthorityOnly)
	void SetLevel(uint8 NewLevel);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void Heal(float Value);

	FORCEINLINE uint8 GetLevel() const noexcept { return Level; }

	FORCEINLINE float GetHealth() const noexcept { return Health; }
	FORCEINLINE float GetMaxHealth() const noexcept { return MaxHealth; }

	FORCEINLINE float GetRunSpeed() const noexcept { return RunSpeed; }
	FORCEINLINE float GetWalkSpeed() const noexcept { return WalkSpeed; }
	FORCEINLINE float GetLockSpeed() const noexcept { return LockSpeed; }

private:
	void BeginPlay() override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetLevel(uint8 NewLevel);

	void ServerSetLevel_Implementation(uint8 NewLevel);
	FORCEINLINE bool ServerSetLevel_Validate(uint8 NewLevel) const { return true; }

	UFUNCTION()
	void OnRep_Level();

public:
	FOnChangedLevel OnChangedLevel;

private:
	UPROPERTY(EditAnywhere, Category = Data, meta = (AllowPrivateAccess = true))
	uint8 StatKey;

	UPROPERTY(ReplicatedUsing = OnRep_Level, EditAnywhere, BlueprintSetter = SetLevel, Category = Data, meta = (AllowPrivateAccess = true))
	uint8 Level;

	UPROPERTY(Replicated, Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = true))
	float Health;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Health, meta = (AllowPrivateAccess = true))
	float MaxHealth;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Speed, meta = (AllowPrivateAccess = true))
	float RunSpeed;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Speed, meta = (AllowPrivateAccess = true))
	float WalkSpeed;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = Speed, meta = (AllowPrivateAccess = true))
	float LockSpeed;

	UPROPERTY()
	class UDataTable* StatDataTable;
};
