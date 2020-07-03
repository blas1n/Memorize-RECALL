// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff/Buff.h"
#include "Misc/TVariant.h"
#include "Faint.generated.h"

UCLASS(BlueprintType)
class PROJECTR_API UFaint final : public UBuff
{
	GENERATED_BODY()
	
private:
	void Initialize() override;

	void BeginBuff() override;
	void EndBuff() override;

	bool IsActivate_Implementation() const override;

private:
	UPROPERTY()
	class AController* Controller;

	uint8 Count;
	uint8 bIsPlayer : 1;
};
