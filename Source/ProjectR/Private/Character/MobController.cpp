// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/MobController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Curves/CurveFloat.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Data/MobData.h"
#include "TimerManager.h"

void AMobController::InitializeLogic(const FLogicData& LogicData)
{
	auto* MobData = Cast<UMobData>(LogicData.AdditionalData);
	if (!MobData) return;

	DetectionCurve = MobData->DetectionCurve;
	DetectionRadius = LogicData.SightRadius;
	ImmediateDetectionRadius = MobData->ImmediateDetectionRadius;
	DecreaseDetectionValue = MobData->DecreaseDetectionValue;
	QuestDuration = MobData->QuestDuration;

	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AMobController::OnPerceptionUpdated);
}

void AMobController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!GetPawn() || ChaseTarget) return;

	auto* MyBlackboard = GetBlackboardComponent();
	if (!MyBlackboard) return;

	if (SeeEnemys.Num() > 0)
	{
		UpdateDetection(DeltaSeconds);
		if (DetectionValue >= 1.0f)
		{
			ChaseTarget = SeeEnemys[0];
			MyBlackboard->SetValueAsObject(TEXT("TargetActor"), ChaseTarget);
			DetectionValue = 0.0f;
		}
	}
	else
	{
		DetectionValue -= DecreaseDetectionValue * DeltaSeconds;
		if (DetectionValue <= 0.0f)
			DetectionValue = 0.0f;
	}

	MyBlackboard->SetValueAsBool(TEXT("IsDetection"), DetectionValue > 0.0f);
}

void AMobController::UpdateDetection(float DeltaSeconds)
{
	for (AActor* Enemy : SeeEnemys)
	{
		const FVector PawnLocation = GetPawn()->GetActorLocation();
		const FVector EnemyLocation = Enemy->GetActorLocation();
		const float Distance = (PawnLocation - EnemyLocation).Size();

		if (Distance <= ImmediateDetectionRadius)
		{
			DetectionValue = 1.0f;
			return;
		}

		const float CurveValue = FMath::GetMappedRangeValueUnclamped(
			FVector2D{ ImmediateDetectionRadius, DetectionRadius },
			FVector2D{ 0.0f, 1.0f },
			Distance
		);

		DetectionValue += DetectionCurve->GetFloatValue(CurveValue) * DeltaSeconds;
		if (DetectionValue >= 1.0f)
			return;
	}
}

void AMobController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		FActorPerceptionBlueprintInfo Info;
		PerceptionComponent->GetActorsPerception(Actor, Info);

		for (const auto& Stimulus : Info.LastSensedStimuli)
			if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
				OnSightUpdated(Actor, Stimulus);
	}
}

void AMobController::OnSightUpdated(AActor* Actor, const FAIStimulus& Stimulus)
{
	auto* TargetPawn = Cast<APawn>(Actor);
	if (!TargetPawn) return;
	
	auto* TargetController = TargetPawn->GetController();
	if (!TargetController) return;

	if (GetTeamAttitudeTowards(*TargetController) != ETeamAttitude::Hostile)
		return;

	if (Stimulus.IsActive())
	{
		if (ChaseTarget == Actor)
		{
			GetWorldTimerManager().ClearTimer(QuestTimer);
		}
	
		SeeEnemys.Add(Actor);
	}
	else
	{
		if (ChaseTarget == Actor)
		{
			GetWorldTimerManager().SetTimer(QuestTimer, [this]
			{
				if (auto* MyBlackboard = GetBlackboardComponent())
					MyBlackboard->SetValueAsObject(TEXT("TargetActor"), nullptr);
				
				ChaseTarget = nullptr;
			}, QuestDuration, false);
		}

		SeeEnemys.Remove(Actor);
	}
}
