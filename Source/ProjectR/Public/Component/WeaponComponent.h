// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/CombatState.h"
#include "Data/VisualData.h"
#include "WeaponComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDodge);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopSkill);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnableCombo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDisableCombo);

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

	UFUNCTION(BlueprintSetter)
	void SetLevel(uint8 InLevel);

	void OnEndSkill();

	void SetWeaponComponent(class UWeaponMeshComponent* InRightWeapon,
		UWeaponMeshComponent* InLeftWeapon) noexcept;

	void SetComponents(const TArray<class UPrimitiveComponent*>&Components);

	FORCEINLINE const FAnimData& GetAnimData() const noexcept { return VisualData.AnimData; }
	FORCEINLINE float GetWeaponSwapDuration() const noexcept { return WeaponSwapDuration; }
	FORCEINLINE int32 GetWeaponNum() const noexcept { return Weapons.Num(); }
	FORCEINLINE uint8 GetWeaponIndex() const noexcept { return WeaponIndex; }
	FORCEINLINE bool IsCheckingCombo() const noexcept { return bNowCombo; }
	FORCEINLINE bool IsBlockSkill() const noexcept { return bBlockSkill; }

private:
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void InitializeComponent() override;
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	
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

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetLevel(uint8 InLevel);

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

	void ServerSetLevel_Implementation(uint8 InLevel);
	FORCEINLINE bool ServerSetLevel_Validate(uint8 InLevel) const noexcept { return InLevel < 10; }

	void EquipWeapon(class UWeapon* NewWeapon);
	void Initialize();

	UFUNCTION()
	void OnRep_VisualData();

	UFUNCTION()
	void Detach();

public:
	UPROPERTY(BlueprintAssignable)
	FOnAttack OnAttack;

	UPROPERTY(BlueprintAssignable)
	FOnDodge OnDodge;

	UPROPERTY(BlueprintAssignable)
	FOnStopSkill OnStopSkill;

	UPROPERTY(BlueprintAssignable)		
	FOnEnableCombo OnEnableCombo;

	UPROPERTY(BlueprintAssignable)
	FOnDisableCombo OnDisableCombo;

private:
	UPROPERTY(EditAnywhere, Category = Data, meta = (AllowPrivateAccess = true))
	TArray<uint8> Keies;

	UPROPERTY(EditAnywhere, Category = Data, meta = (AllowPrivateAccess = true, ClmapMin = "0.01"))
	float WeaponSwapDuration;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UWeaponMeshComponent* RightWeapon;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UWeaponMeshComponent* LeftWeapon;

	UPROPERTY(Transient)
	TArray<UWeapon*> Weapons;

	UPROPERTY(Transient)
	UWeapon* NoWeapon;
	
	UPROPERTY(Transient)
	class USkillContext* SkillContext;

	UPROPERTY(ReplicatedUsing = OnRep_VisualData, Transient)
	FVisualData VisualData;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	ECombatState CombatState;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintSetter = SetLevel, meta = (AllowPrivateAccess = true))
	uint8 Level;

	uint8 WeaponIndex;
	uint8 SkillIndex;

	UPROPERTY(Replicated, Transient)
	uint8 bNowCombo : 1;

	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	uint8 bBlockSkill : 1;
};
