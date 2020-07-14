// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ProjectRAIController.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Patrol, Detection, Chase, Quest,
};

UCLASS()
class PROJECTR_API AProjectRAIController final : public AAIController
{
	GENERATED_BODY()

public:
	AProjectRAIController();

	void InitLogic(const struct FLogicData& LogicData);

	UFUNCTION(BlueprintCallable)
	void SetAIState(EAIState NewAIState);

private:
	void Tick(float DeltaSeconds) override;

	void OnPossess(APawn* InPawn) override;
	void OnUnPossess() override;

	void InitBlackboard(const FLogicData& LogicData);

	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	void OnSightUpdated(AActor* Actor, const struct FAIStimulus& Stimulus);
	void SetFloorLocation(const FVector& BaseLocation);

	UFUNCTION()
	void OnDeath(AController* LastInstigator);

private:
	UPROPERTY()
	AActor* TargetActor;
	
	float DetectionValue;
	float DetectionIncrease;
	float DetectionDecrease;

	EAIState AIState;
	uint8 bIsSeePlayer : 1;
};
