// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Buff.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnApplied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReleased);

UCLASS(Abstract, Blueprintable)
class PROJECTR_API UBuff : public UObject
{
	GENERATED_BODY()

public:
	virtual void BeginPlay();
	virtual void EndPlay();
	virtual void Tick(float DeltaSeconds);
	
	UFUNCTION(BlueprintCallable)
	void Apply();

	UFUNCTION(BlueprintCallable)
	void Release();

	UFUNCTION(BlueprintCallable)
	void Block();

	UFUNCTION(BlueprintCallable)
	void Unblock();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsActivate() const;

	UFUNCTION(BlueprintCallable)
	bool IsBlocked() const;

	UWorld* GetWorld() const override;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "BeginPlay"))
	void RecieveBeginPlay();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "EndPlay"))
	void RecieveEndPlay();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Tick"))
	void RecieveTick(float DeltaSeconds);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnApply"))
	void RecieveOnApply();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnRelease"))
	void RecieveOnRelease();

	virtual void OnApply() {}
	virtual void OnRelease() {}

	virtual bool IsActivate_Implementation() const { return false; }

	FORCEINLINE class AProjectRCharacter* GetTarget() const noexcept { return Target; }

public:
	UPROPERTY(BlueprintAssignable)
	FOnApplied OnApplied;

	UPROPERTY(BlueprintAssignable)
	FOnReleased OnReleased;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	AProjectRCharacter* Target;

	uint8 bIsBlock : 1;
	uint8 bIsActivateWithoutBlock : 1;
};
