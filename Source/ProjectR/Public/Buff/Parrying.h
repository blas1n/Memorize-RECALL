// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff/Buff.h"
#include "Parrying.generated.h"

UCLASS(BlueprintType)
class PROJECTR_API UParrying final : public UBuff
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool ParryIfCan(int32 Damage, class AController*
		Instigator, class AProjectRCharacter* Causer);
	
	UFUNCTION(BlueprintCallable)
	void SetParryingObject(UObject* Object);

private:
	void OnApply() override;
	void OnRelease() override;

	bool IsActivate_Implementation() const override;

private:
	UPROPERTY()
	UObject* ParryingObject;

	uint8 bIsParrying : 1;
};
