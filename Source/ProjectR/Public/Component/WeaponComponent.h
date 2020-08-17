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
	void Parry();

	UFUNCTION(BlueprintCallable)
	void StopSkill();

	UFUNCTION(BlueprintCallable)
	void SwapWeapon(uint8 Index);

	UFUNCTION(BlueprintCallable)
	void AddWeapon(uint8 Index, int32 Kesy);

	UFUNCTION(BlueprintCallable)
	void Execute();

	UFUNCTION(BlueprintCallable)
	void BeginExecute();

	UFUNCTION(BlueprintCallable)
	void EndExecute();

	UFUNCTION(BlueprintCallable)
	void TickExecute(float DeltaSeconds);

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
	void ServerParry();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopSkill();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSwapWeapon(uint8 Index);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerAddWeapon(uint8 Index, int32 Key);

	UFUNCTION(Client, Reliable)
	void ClientEndCast();

	UFUNCTION(Client, Reliable)
	void ClientResetParrying();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEquipWeapon(TSubclassOf<UAnimInstance> UnlinkAnim);

	void ServerAttack_Implementation(bool bIsStrongAttack);
	FORCEINLINE bool ServerAttack_Validate(bool bIsStrongAttack) const noexcept { return true; }

	void ServerParry_Implementation();
	FORCEINLINE bool ServerParry_Validate() const noexcept { return true; }

	void ServerStopSkill_Implementation();
	FORCEINLINE bool ServerStopSkill_Validate() const noexcept { return true; }

	void ServerSwapWeapon_Implementation(uint8 Index);
	FORCEINLINE bool ServerSwapWeapon_Validate(uint8 Index) const noexcept { return Weapons.Num() > Index; }

	void ServerAddWeapon_Implementation(uint8 Index, int32 Key);
	FORCEINLINE bool ServerAddWeapon_Validate(uint8 Index, int32 Key) const noexcept { return true; }

	void ClientEndCast_Implementation();
	void ClientResetParrying_Implementation();

	FORCEINLINE void MulticastEquipWeapon_Implementation
		(TSubclassOf<UAnimInstance> UnlinkAnim) { ApplyWeapon(UnlinkAnim); }

	UStaticMeshComponent* CreateWeaponComponent(const FName& Name);
	void EquipWeapon(class UWeapon* NewWeapon, bool bNeedUnequip);
	void Initialize();

	void ApplyWeapon(TSubclassOf<UAnimInstance> UnlinkAnim);

	UFUNCTION()
	void Detach();

	void DetachOnce(class UStaticMeshComponent* Weapon);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* RightWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UStaticMeshComponent* LeftWeapon;

	UPROPERTY(EditAnywhere, Category = Data, meta = (AllowPrivateAccess = true))
	TArray<int32> Keies;

	UPROPERTY(Transient)
	TArray<UWeapon*> Weapons;

	UPROPERTY(Transient)
	UWeapon* NoWeapon;
	
	UPROPERTY(Replicated)
	class USkillContext* SkillContext;

	UPROPERTY(Replicated)
	FVisualData VisualData;

	uint8 WeaponIndex;
	uint8 SkillIndex;
	uint8 SkillCount;

	uint8 bIsCasting : 1;
	uint8 bUseParry : 1;
	uint8 bNowParry : 1;
};
