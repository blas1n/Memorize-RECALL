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

	FORCEINLINE USkill* GetSkill() const { return Skill; }

private:
	void OnRelease() override;

	bool IsActivate_Implementation() const override;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	USkill* Skill;
};