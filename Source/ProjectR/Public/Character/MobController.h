// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ProjectRAIController.h"
#include "MobController.generated.h"

UCLASS()
class PROJECTR_API AMobController : public AProjectRAIController
{
	GENERATED_BODY()
	
private:
	void InitializeLogic(const FLogicData& LogicData) override;
	void Tick(float DeltaSeconds) override;

	void UpdateDetection(float DeltaSeconds);

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	void OnSightUpdated(AActor* Actor, const struct FAIStimulus& Stimulus);

private:
	UPROPERTY()
	AActor* ChaseTarget;

	UPROPERTY()
	TArray<AActor*> SeeEnemys;

	UPROPERTY()
	class UCurveFloat* DetectionCurve;

	FTimerHandle QuestTimer;

	float DetectionValue;
	float DetectionRadius;
	float ImmediateDetectionRadius;
	float DecreaseDetectionValue;
	float QuestDuration;
};
