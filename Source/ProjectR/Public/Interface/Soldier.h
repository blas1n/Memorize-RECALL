// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Soldier.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class USoldier : public UInterface
{
	GENERATED_BODY()
};

class PROJECTR_API ISoldier
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Start();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Pause();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetTarget(class AActor* Target);
};
