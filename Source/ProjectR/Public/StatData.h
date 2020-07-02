// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StatData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FStatData : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealthHeal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxEnergy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EnergyHeal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CrouchSpeed;
};