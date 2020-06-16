// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Buff.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTR_API UBuff final : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ApplyBuff(class AProjectRCharacter* Target, float Duration);

	FORCEINLINE const TSubclassOf<class UBuffStorage>&
	GetBuffStorageClass() const { return BuffStorageClass; }

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnBeginBuff(AProjectRCharacter* Target, UBuffStorage* BuffStorage);

	UFUNCTION(BlueprintImplementableEvent)
	void OnEndBuff(AProjectRCharacter* Target, UBuffStorage* BuffStorage);

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = BuffStorage, meta = (AllowPrivateAccess = true))
	TSubclassOf<UBuffStorage> BuffStorageClass;
};
