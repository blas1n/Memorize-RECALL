// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Skill/SingleAttack.h"
#include "Leap.generated.h"

UCLASS()
class PROJECTR_API ULeap final : public USingleAttack
{
	GENERATED_BODY()

public:
	ULeap();

private:
	void Begin(USkillContext* InContext, const UDataAsset* Data) override;
	void Tick(float DeltaTime) override;
	void End() override;

	void SetData(const UDataAsset* Data);
	void InitSpline();
	void UpdateSpline();
	void UpdateLeap(float DeltaTime);

private:
	UPROPERTY()
	class USpline* Spline;

	UPROPERTY(Transient)
	class AActor* Target;

	FVector StartLocation;

	float MaxHeight;
	float LeapDuration;
	float AttackRange;
	float MaxHeightRatio;
	float LeapTimeRatio;

	int32 Idx;
	float Alpha;
	uint8 bIsLeapEnded : 1;
};
