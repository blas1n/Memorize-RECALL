// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/Skill.h"
#include "Interface/StateExecutable.h"
#include "SingleAttack.generated.h"

UCLASS()
class PROJECTR_API USingleAttack : public USkill, public IStateExecutable
{
	GENERATED_BODY()
	
protected:
	void Begin(USkillContext* InContext, const UDataAsset* Data) override;
	void End() override;

	bool CanUseSkill_Implementation() const override;

	void BeginExecute_Implementation() override;
	void EndExecute_Implementation() override;

private:
	bool IsValidInput(USkillContext* InContext, const UDataAsset* Data) const;

	UFUNCTION()
	void OnHit(AActor* Target);

	UFUNCTION()
	void OnAnimationEnded();

private:
	UPROPERTY(Transient)
	TArray<AActor*> AttackedActors;

	UPROPERTY(Transient)
	USkillContext* Context;

	UPROPERTY(Transient)
	class UAnimMontage* Animation;

	float Damage;
	int32 AttackPart;
};
