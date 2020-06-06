// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Skill.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTR_API ASkill : public AActor
{
	GENERATED_BODY()
	
public:	
	ASkill();

	UFUNCTION(BlueprintNativeEvent)
	void Initialize(class AWeapon* InWeapon);

	UFUNCTION(BlueprintCallable)
	bool UseSkill();

protected:
	void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnSkill();

	UFUNCTION(BlueprintNativeEvent)
	bool CanUseSkill();

	UFUNCTION(BlueprintCallable)
	bool IsNotCoolTime() const;

	UFUNCTION(BlueprintCallable)
	void ApplyCooltime();

	FORCEINLINE class AProjectRCharacter* GetUser() noexcept { return User; }
	FORCEINLINE const AProjectRCharacter* GetUser() const noexcept { return User; }

private:
	FORCEINLINE void Initialize_Implementation(AWeapon* InWeapon) { Weapon = InWeapon; }
	FORCEINLINE bool CanUseSkill_Implementation() { return true; }

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	class AProjectRCharacter* User;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	AWeapon* Weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Limit, meta = (AllowPrivateAccess = true))
	float CoolTime;

	float NextUseTime;
};