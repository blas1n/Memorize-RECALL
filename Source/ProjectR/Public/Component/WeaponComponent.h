// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/VisualData.h"
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
	void Dodge();
	
	UFUNCTION(BlueprintCallable)
	void StopSkill();

	UFUNCTION(BlueprintCallable)
	void SwapWeapon(uint8 Index);

	UFUNCTION(BlueprintCallable)
	void ChangeWeapon(uint8 Index, int32 Key);

	UFUNCTION(BlueprintCallable)
	void AddWeapon(int32 Key);

	UFUNCTION(BlueprintCallable)
	void Execute();

	UFUNCTION(BlueprintCallable)
	void BeginExecute();

	UFUNCTION(BlueprintCallable)
	void EndExecute();

	UFUNCTION(BlueprintCallable)
	void EnableCombo();

	UFUNCTION(BlueprintCallable)
	void DisableCombo();

	void OnEndSkill();

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
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAttack(bool bIsStrongAttack);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDodge();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopSkill();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSwapWeapon(uint8 Index);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeWeapon(uint8 Index, int32 Key);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddWeapon(int32 Key);

	void ServerAttack_Implementation(bool bIsStrongAttack);
	FORCEINLINE bool ServerAttack_Validate(bool bIsStrongAttack) const noexcept { return true; }

	void ServerDodge_Implementation();
	FORCEINLINE bool ServerDodge_Validate() const noexcept { return true; }

	void ServerStopSkill_Implementation();
	FORCEINLINE bool ServerStopSkill_Validate() const noexcept { return true; }

	void ServerSwapWeapon_Implementation(uint8 Index);
	FORCEINLINE bool ServerSwapWeapon_Validate(uint8 Index) const noexcept { return Weapons.Num() > Index; }

	void ServerChangeWeapon_Implementation(uint8 Index, int32 Key);
	FORCEINLINE bool ServerChangeWeapon_Validate(uint8 Index, int32 Key) const noexcept { return true; }

	void ServerAddWeapon_Implementation(int32 Key);
	FORCEINLINE bool ServerAddWeapon_Validate(int32 Key) const noexcept { return true; }

	UStaticMeshComponent* CreateWeaponComponent(const FName& Name);
	void EquipWeapon(class UWeapon* NewWeapon);
	void Initialize();

	UFUNCTION()
	void OnRep_VisualData();

	UFUNCTION()
	void Detach();

	void DetachOnce(class UStaticMeshComponent* Weapon);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* RightWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* LeftWeapon;

	UPROPERTY(EditAnywhere, Category = Data, meta = (AllowPrivateAccess = true))
	TArray<uint8> Keies;

	UPROPERTY(Transient)
	TArray<UWeapon*> Weapons;

	UPROPERTY(Transient)
	UWeapon* NoWeapon;
	
	UPROPERTY(Transient)
	class USkillContext* SkillContext;

	UPROPERTY(ReplicatedUsing = OnRep_VisualData, Transient)
	FVisualData VisualData;

	uint8 WeaponIndex;
	uint8 SkillIndex;

	uint8 bIsCasting : 1;
	uint8 bNowCombo : 1;
	uint8 bNowDodge : 1;
};
