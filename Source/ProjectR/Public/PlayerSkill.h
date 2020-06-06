// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill.h"
#include "PlayerSkill.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTR_API APlayerSkill final : public ASkill
{
	GENERATED_BODY()
	
public:
	APlayerSkill();

protected:
	UFUNCTION(BlueprintCallable)
	bool IsEnoughEnergy() const;

	UFUNCTION(BlueprintCallable)
	void ApplyEnergy();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Limit, meta = (AllowPrivateAccess = true))
	int32 UseEnergy;
};
