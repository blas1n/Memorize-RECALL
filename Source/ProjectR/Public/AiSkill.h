// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill.h"
#include "AiSkill.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTR_API AAiSkill : public ASkill
{
	GENERATED_BODY()
	
public:
	AAiSkill();

protected:
	UFUNCTION(BlueprintCallable)
	bool IsInRange() const;

private:
	void BeginPlay() override;

private:
	UPROPERTY()
	class APawn* Player;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Limit, meta = (AllowPrivateAccess = true))
	float Range;
};
