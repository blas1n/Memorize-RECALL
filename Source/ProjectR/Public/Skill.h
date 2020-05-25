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
	FORCEINLINE void Initialize(class AWeapon* InWeapon) { Weapon = InWeapon; }

	UFUNCTION(BlueprintImplementableEvent)
	void OnPress();

	UFUNCTION(BlueprintImplementableEvent)
	void OnRelease();

protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UseSkill();

	UFUNCTION(BlueprintCallable)
	bool CheckAndApplyLimit();

private:
	void BeginPlay() override;
	bool IsCoolTime() const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	class AProjectRCharacter* User;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	AWeapon* Weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Limit, meta = (AllowPrivateAccess = true))
	int32 UseEnergy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Limit, meta = (AllowPrivateAccess = true))
	float CoolTime;

	float NextUseTime;
};