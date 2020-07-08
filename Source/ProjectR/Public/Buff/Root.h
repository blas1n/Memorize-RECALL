// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff/Buff.h"
#include "Root.generated.h"

UCLASS(BlueprintType)
class PROJECTR_API URoot final : public UBuff
{
	GENERATED_BODY()
	
private:
	void OnApply() override;
	void OnRelease() override;

	bool IsActivate_Implementation() const override;

private:
	uint8 Count;
};
