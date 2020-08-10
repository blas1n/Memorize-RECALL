// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff/Buff.h"
#include "Parry.generated.h"

UCLASS(BlueprintType)
class PROJECTR_API UParry final : public UBuff
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool ParryIfCan(float Damage, class AController*
		Instigator, class APRCharacter* Causer);
	
	UFUNCTION(BlueprintCallable)
	void SetParryObject(UObject* Object);

private:
	void OnApply() override;
	void OnRelease() override;

	bool IsActivate_Implementation() const override;

private:
	UPROPERTY()
	UObject* ParryObject;

	uint8 bIsParry : 1;
};
