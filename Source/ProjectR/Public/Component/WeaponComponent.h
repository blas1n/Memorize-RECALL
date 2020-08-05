// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTR_API UWeaponComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponComponent();

	UFUNCTION(BlueprintCallable)
	void Attack(bool bIsStrongAttack);

	UFUNCTION(BlueprintCallable)
	void Parry();

	UFUNCTION(BlueprintCallable)
	void StopSkill();

	UFUNCTION(BlueprintCallable)
	void SwapWeapon(uint8 Index);

	UFUNCTION(BlueprintCallable)
	void AddWeapon(uint8 Index, int32 Key);

	FORCEINLINE class UStaticMeshComponent* GetRightWeapon() const noexcept { return RightWeapon; }
	FORCEINLINE UStaticMeshComponent* GetLeftWeapon() const noexcept { return LeftWeapon; }

	FORCEINLINE int32 GetWeaponNum() const noexcept { return Weapons.Num(); }
	FORCEINLINE uint8 GetWeaponIndex() const noexcept { return WeaponIndex; }

private:
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void InitializeComponent() override;
	void BeginPlay() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttack(bool bIsStrongAttack);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerParry();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopSkill();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSwapWeapon(uint8 Index);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddWeapon(uint8 Index, int32 Key);

	void ServerAttack_Implementation(bool bIsStrongAttack);
	bool ServerAttack_Validate(bool bIsStrongAttack);

	void ServerParry_Implementation();
	bool ServerParry_Validate();

	void ServerStopSkill_Implementation();
	bool ServerStopSkill_Validate();

	void ServerSwapWeapon_Implementation(uint8 Index);
	bool ServerSwapWeapon_Validate(uint8 Index);

	void ServerAddWeapon_Implementation(uint8 Index, int32 Key);
	bool ServerAddWeapon_Validate(uint8 Index, int32 Key);

	UStaticMeshComponent* CreateWeaponComponent(const FName& Name);
	void EquipWeapon(class UWeapon* NewWeapon, bool bNeedUnequip);
	void Initialize();

	UFUNCTION()
	void Detach();

	void DetachOnce(class UStaticMeshComponent* Weapon);

private:
	UPROPERTY(EditAnywhere, Category = Data, meta = (AllowPrivateAccess = true))
	TArray<int32> Keies;

	UPROPERTY()
	TArray<UWeapon*> Weapons;

	UPROPERTY(Transient)
	UWeapon* NoWeapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* RightWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* LeftWeapon;

	UPROPERTY()
	class UWeaponContext* WeaponContext;

	uint8 WeaponIndex;
	uint8 SkillIndex;
};
