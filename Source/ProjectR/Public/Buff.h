// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Buff.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTR_API UBuff : public UObject
{
	GENERATED_BODY()

public:
	void Initialize();
	
	UFUNCTION(BlueprintCallable)
	void ApplyBuff(float Duration);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnBeginBuff();

	UFUNCTION(BlueprintImplementableEvent)
	void OnEndBuff();

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class AProjectRCharacter* Target;
};
