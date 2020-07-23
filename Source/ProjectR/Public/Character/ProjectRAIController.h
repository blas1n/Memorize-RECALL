// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "ProjectRAIController.generated.h"

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Patrol, Detection, Chase,
};

UCLASS()
class PROJECTR_API AProjectRAIController final : public AAIController
{
	GENERATED_BODY()

public:
	AProjectRAIController();

	void InitLogic(const TAssetPtr<UBehaviorTree>& BehaviorTree, const struct FLogicData& LogicData);

private:
	void BeginPlay() override;

	void OnPossess(APawn* InPawn) override;
	void OnUnPossess() override;

	void InitBlackboard(const FLogicData& LogicData);

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void OnDeath(AController* LastInstigator);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UNavigationInvokerComponent* NavigationInvoker;

	UPROPERTY()
	class UCurveFloat* DetectionCurve;

	float DetectionValue;
	float ImmediateDetectionRadius;

	float SightRadius;
	float LockRadiusSquared;

	EAIState AIState;
	uint8 bIsSeePlayer : 1;
};
