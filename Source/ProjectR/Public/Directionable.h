// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Directionable.generated.h"

UINTERFACE(Blueprintable, MinimalAPI)
class UDirectionable : public UInterface
{
	GENERATED_BODY()
};

class PROJECTR_API IDirectionable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	FVector2D GetDirection();

	virtual FVector2D GetDirection_Implementaion() { return FVector2D{ 1.0f, 0.0f }; }
};
