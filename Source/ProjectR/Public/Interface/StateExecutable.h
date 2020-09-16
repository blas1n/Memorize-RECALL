// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StateExecutable.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class UStateExecutable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTR_API IStateExecutable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BeginExecute();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EndExecute();

	virtual void BeginExecute_Implementation() {}
	virtual void EndExecute_Implementation() {}
};
