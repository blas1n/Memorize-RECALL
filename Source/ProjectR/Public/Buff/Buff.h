// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NetworkObject.h"
#include "Buff.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnApplied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReleased);

UCLASS(Abstract, Blueprintable)
class PROJECTR_API UBuff : public UNetworkObject
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

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsActivate() const;

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

	FORCEINLINE class APRCharacter* GetTarget() const noexcept { return Target; }

private:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerApply();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRelease();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastApply();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRelease();

	FORCEINLINE void ServerApply_Implementation() { MulticastApply(); }
	FORCEINLINE bool ServerApply_Validate() { return true; }

	FORCEINLINE void ServerRelease_Implementation() { MulticastRelease(); }
	FORCEINLINE bool ServerRelease_Validate() { return true; }

	void MulticastApply_Implementation();
	void MulticastRelease_Implementation();

public:
	UPROPERTY(BlueprintAssignable)
	FOnApplied OnApplied;

	UPROPERTY(BlueprintAssignable)
	FOnReleased OnReleased;

private:
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	APRCharacter* Target;
};
