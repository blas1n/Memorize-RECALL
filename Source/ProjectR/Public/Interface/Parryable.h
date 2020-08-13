// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Parryable.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class UParryable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTR_API IParryable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool IsParryable(float Damage, class APRCharacter* Causer) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Parry(float Damage, APRCharacter* Causer);
};
