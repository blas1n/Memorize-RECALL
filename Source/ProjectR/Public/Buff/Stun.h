// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff/Buff.h"
#include "Misc/TVariant.h"
#include "Stun.generated.h"

UCLASS(BlueprintType)
class PROJECTR_API UStun final : public UBuff
{
	GENERATED_BODY()
	
private:
	void OnApply() override;
	void OnRelease() override;

	bool IsActivate_Implementation() const override;

private:
	uint8 Count;
};
