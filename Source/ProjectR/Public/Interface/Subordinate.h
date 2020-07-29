// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/Soldier.h"
#include "Subordinate.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class USubordinate : public USoldier
{
	GENERATED_BODY()
};

class PROJECTR_API ISubordinate : public ISoldier
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Join(UObject* Superior);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Discharge(UObject* Superior);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ReceiveOrder(const FName& Message, UObject* Data);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SendReport(bool Result);
};
