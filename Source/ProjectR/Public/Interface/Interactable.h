// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTR_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(class APRCharacter* Target);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FText GetInteractName() const;

protected:
	virtual void Execute_Implementation(APRCharacter* Target) {}
	virtual FText GetInteractName_Implementation() const { return FText{}; }
};
