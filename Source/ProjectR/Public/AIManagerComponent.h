// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIManagerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTR_API UAIManagerComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	const TArray<FName>& GetWeaponNames();

private:
	void BeginPlay() override;

	void Initialize();

private:
	TArray<FName> WeaponNames;

	uint8 bIsInit : 1;
};
