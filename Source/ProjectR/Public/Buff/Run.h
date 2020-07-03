// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff/Buff.h"
#include "Run.generated.h"

UCLASS()
class PROJECTR_API URun final : public UBuff
{
	GENERATED_BODY()

private:
	void BeginBuff() override;
	void EndBuff() override;

	bool IsActivate_Implementation() const override;

private:
	uint8 Count;
};
