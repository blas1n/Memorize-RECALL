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
	void Initialize();

	UFUNCTION(BlueprintImplementableEvent)
	void Release();

	UFUNCTION(BlueprintCallable)
	void Use();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanUse();

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Initialize"))
	void OnInitialize();

	UFUNCTION(BlueprintImplementableEvent)
	void OnUse();

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	class UActorComponent* NewComponent(TSubclassOf<UActorComponent> Class);

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	bool IsEnoughEnergy() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void ApplyEnergy();

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	bool IsNotCoolTime() const;

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void ApplyCooltime();

	FORCEINLINE class AProjectRCharacter* GetUser() noexcept { return User; }
	FORCEINLINE const AProjectRCharacter* GetUser() const noexcept { return User; }

	FORCEINLINE class UWeapon* GetWeapon() noexcept { return Weapon; }
	FORCEINLINE const UWeapon* GetWeapon() const noexcept { return Weapon; }

private:
	FORCEINLINE bool CanUse_Implementation() { return true; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	AProjectRCharacter* User;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	UWeapon* Weapon;

	UPROPERTY(EditDefaultsOnly, Category = Limit, meta = (AllowPrivateAccess = true))
	uint8 UseEnergy;

	UPROPERTY(EditDefaultsOnly, Category = Limit, meta = (AllowPrivateAccess = true))
	float CoolTime;

	float NextUseTime;
};
