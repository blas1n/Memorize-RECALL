// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff/Buff.h"
#include "Cast.generated.h"

UCLASS()
class PROJECTR_API UCast : public UBuff
{
	GENERATED_BODY()
	
private:
	void BeginBuff() override;
	void EndBuff() override;

	bool IsActivate_Implementation() const override;

private:
	uint8 bIsLocked : 1;
};
