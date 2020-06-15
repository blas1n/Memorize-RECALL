// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Buff.generated.h"

UCLASS(Abstract, BlueprintType)
class PROJECTR_API UBuff final : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ApplyBuff(class AProjectRCharacter* Target, float Duration);

	UFUNCTION(BlueprintImplementableEvent)
	class UBuffStorage* CreateStorage() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (WorldContext = WorldContext))
	void OnBeginBuff(UObject* WorldContext, AProjectRCharacter* Target, UBuffStorage* BuffStorage);

	UFUNCTION(BlueprintImplementableEvent, meta = (WorldContext = WorldContext))
	void OnEndBuff(UObject* WorldContext, AProjectRCharacter* Target, UBuffStorage* BuffStorage);
};
