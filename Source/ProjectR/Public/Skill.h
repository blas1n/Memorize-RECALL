// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Skill.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnComponentHandler, class UActorComponent*, Component);

USTRUCT(Atomic, BlueprintType)
struct FComponentInitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UActorComponent> Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FOnComponentHandler Handler;
};

UCLASS(Abstract, Blueprintable)
class PROJECTR_API USkill final : public UObject
{
	GENERATED_BODY()
	
public:
	void BeginPlay();

	UFUNCTION(BlueprintImplementableEvent)
	void EndPlay();

	UFUNCTION(BlueprintCallable)
	void Start();

	UFUNCTION(BlueprintCallable)
	void End();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanUse() const;

	UFUNCTION(BlueprintImplementableEvent)
	TArray<FComponentInitData> GetNeedComponents();

	UWorld* GetWorld() const override;

	FORCEINLINE int32 GetPriority() const noexcept { return Priority; }

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "BeginPlay"))
	void ReceiveBeginPlay();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Start"))
	void ReceiveStart();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "End"))
	void ReceiveEnd();

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	bool IsNotCoolTime() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void ApplyCooltime();

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	bool IsEnoughEnergy() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void ApplyEnergy();

	FORCEINLINE class AProjectRCharacter* GetUser() noexcept { return User; }
	FORCEINLINE const AProjectRCharacter* GetUser() const noexcept { return User; }

	FORCEINLINE class UWeapon* GetWeapon() noexcept { return Weapon; }
	FORCEINLINE const UWeapon* GetWeapon() const noexcept { return Weapon; }

private:
	bool CanUse_Implementation() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	AProjectRCharacter* User;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	UWeapon* Weapon;

	UPROPERTY(EditDefaultsOnly, Category = Data, meta = (AllowPrivateAccess = true))
	int32 Priority;

	UPROPERTY(EditDefaultsOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float CoolTime;

	UPROPERTY(EditDefaultsOnly, Category = Data, meta = (AllowPrivateAccess = true))
	uint8 UseEnergy;

	float NextUseTime;
};
