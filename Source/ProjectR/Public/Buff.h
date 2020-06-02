// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Buff.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTR_API ABuff : public AActor
{
	GENERATED_BODY()
	
public:
	ABuff();

	UFUNCTION(BlueprintCallable)
	void ApplyBuff(class AProjectRCharacter* InTarget, float Duration);

	UFUNCTION(BlueprintImplementableEvent)
	class UBuffStorage* CreateStorage() const;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnBuff(UBuffStorage* BuffStorage);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	AProjectRCharacter* Target;
};