// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Parryable.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UParryable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTR_API IParryable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool IsParryable(int32 Damage, class AController* Instigator,
		class AProjectRCharacter* Causer) const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Parry(int32 Damage, AController* Instigator, AProjectRCharacter* Causer);
};
