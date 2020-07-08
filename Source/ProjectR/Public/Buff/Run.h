// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff/Buff.h"
#include "Run.generated.h"

UCLASS(BlueprintType)
class PROJECTR_API URun final : public UBuff
{
	GENERATED_BODY()

private:
	void OnApply() override;
	void OnRelease() override;

	bool IsActivate_Implementation() const override;

private:
	uint8 bIsRun : 1;
};
