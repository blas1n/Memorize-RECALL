// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Skill.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTR_API USkill final : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void Use();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanUse();

	UWorld* GetWorld() const override;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnUse();

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
	void Initialize_Implementation();

	FORCEINLINE bool CanUse_Implementation() { return true; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	AProjectRCharacter* User;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	UWeapon* Weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Limit, meta = (AllowPrivateAccess = true))
	float CoolTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Limit, meta = (AllowPrivateAccess = true))
	uint8 UseEnergy;

	float NextUseTime;
};
