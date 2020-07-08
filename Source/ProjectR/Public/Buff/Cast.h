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
	void StopSkill();

	void SetCurSkill(class USkill* Skill);
	bool CanUseSkill(const USkill* Skill) const;

private:
	void OnApply() override;
	void OnRelease() override;

	bool IsActivate_Implementation() const override;

private:
	UPROPERTY()
	class USkill* CurSkill;

	uint8 bIsCasting : 1;
};