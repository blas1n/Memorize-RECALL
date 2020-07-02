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
	virtual void Initialize();
	
	UFUNCTION(BlueprintCallable)
	void ApplyBuff(float Duration);

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Initialize"))
	void RecieveInitialize();

	virtual void BeginBuff() {}
	virtual void EndBuff() {}

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "BeginBuff"))
	void RecieveBeginBuff();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "EndBuff"))
	void RecieveEndBuff();

	FORCEINLINE class AProjectRCharacter* GetTarget() const noexcept { return Target; }

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	AProjectRCharacter* Target;
};
