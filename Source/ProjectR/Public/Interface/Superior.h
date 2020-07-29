// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/Soldier.h"
#include "Superior.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class USuperior : public USoldier
{
	GENERATED_BODY()
};

class PROJECTR_API ISuperior : public ISoldier
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Muster(UObject* Subordinate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Disperse(UObject* Subordinate);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SendOrder(UObject* To, const FName& Message, UObject* Data);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ReceiveReport(UObject* From, bool Result);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	TArray<UObject*> GetSubordinates() const;
};
