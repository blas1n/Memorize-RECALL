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
	virtual void Tick(float DeltaSeconds);
	
	UFUNCTION(BlueprintCallable)
	void ApplyBuff();

	UFUNCTION(BlueprintCallable)
	void ReleaseBuff();

	UFUNCTION(BlueprintCallable)
	void ApplyBuffWithDuration(float Duration);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsActivate() const;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Initialize"))
	void RecieveInitialize();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Tick"))
	void RecieveTick(float DeltaSeconds);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "BeginBuff"))
	void RecieveBeginBuff();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "EndBuff"))
	void RecieveEndBuff();

	virtual void BeginBuff() {}
	virtual void EndBuff() {}

	virtual bool IsActivate_Implementation() const { return false; }

	FORCEINLINE class AProjectRCharacter* GetTarget() const noexcept { return Target; }

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	AProjectRCharacter* Target;
};
