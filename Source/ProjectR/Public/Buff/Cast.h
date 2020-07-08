// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Buff/Buff.h"
#include "Cast.generated.h"

UCLASS(BlueprintType)
class PROJECTR_API UCast final : public UBuff
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void StopCast();

	void CastSkill(class USkill* NewSkill);
	bool CanUseSkill(const USkill* NewSkill) const;

private:
	void OnRelease() override;

	bool IsActivate_Implementation() const override;

private:
	UPROPERTY()
	USkill* Skill;
};