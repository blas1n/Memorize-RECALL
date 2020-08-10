// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ComponentOwner.generated.h"

UINTERFACE(BlueprintType, MinimalAPI)
class UComponentOwner : public UInterface
{
	GENERATED_BODY()
};

class PROJECTR_API IComponentOwner
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class UWeaponComponent* GetWeaponComponent() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	class UStatComponent* GetStatComponent() const;

protected:
	virtual UWeaponComponent* GetWeaponComponent_Implementation() const { return nullptr; }
	virtual UStatComponent* GetStatComponent_Implementation() const { return nullptr; }
};
