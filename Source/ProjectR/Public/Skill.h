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

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Use();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanUse() const;

	UWorld* GetWorld() const override;

	FORCEINLINE bool IsCastSkill() const noexcept { return bIsCastSkill; }

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Initialize"))
	void OnInitialize();

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	class UActorComponent* NewComponent(TSubclassOf<UActorComponent> Class);

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
	FORCEINLINE bool CanUse_Implementation() const { return true; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	AProjectRCharacter* User;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	UWeapon* Weapon;

	UPROPERTY(EditDefaultsOnly, Category = Data, meta = (AllowPrivateAccess = true))
	float CoolTime;

	UPROPERTY(EditDefaultsOnly, Category = Data, meta = (AllowPrivateAccess = true))
	uint8 UseEnergy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (AllowPrivateAccess = true))
	uint8 bIsCastSkill : 1;

	float NextUseTime;
};
