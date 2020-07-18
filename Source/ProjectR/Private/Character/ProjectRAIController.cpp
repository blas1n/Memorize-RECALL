// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Curves/CurveFloat.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AIManagerComponent.h"
#include "Buff/Lock.h"
#include "Buff/Run.h"
#include "Character/ProjectRCharacter.h"
#include "Data/LogicData.h"
#include "BuffLibrary.h"
#include "ProjectRStatics.h"

AProjectRAIController::AProjectRAIController()
	: Super()
{
	bWantsPlayerState = true;

	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));
	
	auto* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISenseConfig_Sight"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	GetPerceptionComponent()->ConfigureSense(*SightConfig);

	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AProjectRAIController::OnPerceptionUpdated);
}

void AProjectRAIController::InitLogic(const TAssetPtr<UBehaviorTree>& BehaviorTree, const FLogicData& LogicData)
{
	UProjectRStatics::AsyncLoad(BehaviorTree, [this, BehaviorTree, LogicData]() mutable
	{
		RunBehaviorTree(BehaviorTree.Get());
		InitBlackboard(LogicData);
	});

	DetectionCurve = LogicData.DetectionCurve;
	ImmediateDetectionRadius = LogicData.ImmediateDetectionRadius;
	DetectionDecreaseValue = LogicData.DetectionDecreaseValue;

	SightRadius = LogicData.SightRadius;
	LockRadiusSquared = FMath::Square(LogicData.LockRadius);

	auto* SightConfig = Cast<UAISenseConfig_Sight>(GetPerceptionComponent()
		->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()));

	SightConfig->SightRadius = LogicData.SightRadius;
	SightConfig->LoseSightRadius = LogicData.LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = LogicData.FOV;
	GetPerceptionComponent()->RequestStimuliListenerUpdate();
}

void AProjectRAIController::SetAIState(EAIState NewAIState)
{
	AIState = NewAIState;
	if (auto* MyBlackboard = GetBlackboardComponent())
		MyBlackboard->SetValueAsEnum(TEXT("AIState"), static_cast<uint8>(AIState));

	DetectionValue = 0.0f;

	auto* MyPawn = GetPawn<AProjectRCharacter>();
	UBuffLibrary::ReleaseBuff<URun>(MyPawn);

	if (AIState == EAIState::Patrol)
		UBuffLibrary::ReleaseBuff<ULock>(MyPawn);
	else
		UBuffLibrary::ApplyBuff<ULock>(MyPawn);
}

void AProjectRAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (TargetActor)
	{
		if (Cast<AProjectRCharacter>(TargetActor)->IsDeath())
		{
			SetAIState(EAIState::Patrol);
			TargetActor = nullptr;
		}
		else
		{
			SetFloorLocation(TargetActor->GetActorLocation());
		}
	}

	if (AIState == EAIState::Detection)
		UpdateDetection(DeltaSeconds);
	else if (AIState == EAIState::Chase)
		UpdateChase();
}

void AProjectRAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Cast<AProjectRCharacter>(InPawn)->OnDeath
		.AddDynamic(this, &AProjectRAIController::OnDeath);
}

void AProjectRAIController::OnUnPossess()
{
	if (GetPawn() != nullptr)
	{
		Cast<AProjectRCharacter>(GetPawn())->OnDeath
			.RemoveDynamic(this, &AProjectRAIController::OnDeath);
	}

	Super::OnUnPossess();
}

void AProjectRAIController::InitBlackboard(const FLogicData& LogicData)
{
	if (auto* MyBlackboard = GetBlackboardComponent())
	{
		MyBlackboard->SetValueAsVector(TEXT("HomeLocation"), GetPawn()->GetActorLocation());
		MyBlackboard->SetValueAsFloat(TEXT("PatrolRadius"), LogicData.PatrolRadius);
		MyBlackboard->SetValueAsFloat(TEXT("QuestRadius"), LogicData.QuestRadius);
	}
}

void AProjectRAIController::UpdateDetection(float DeltaSeconds)
{
	if (bIsSeePlayer)
	{
		auto* MyPawn = GetPawn<AProjectRCharacter>();
		const FVector PawnLocation = MyPawn->GetActorLocation();
		const FVector TargetLocation = TargetActor->GetActorLocation();
		const float Distance = (PawnLocation - TargetLocation).Size();

		if (Distance <= ImmediateDetectionRadius)
		{
			SetAIState(EAIState::Chase);
			return;
		}
		
		const float CurveValue = FMath::GetMappedRangeValueUnclamped(
			FVector2D{ ImmediateDetectionRadius, SightRadius },
			FVector2D{ 0.0f, 1.0f },
			Distance
		);

		DetectionValue += DetectionCurve->GetFloatValue(CurveValue) * DeltaSeconds;
		if (DetectionValue >= 1.0f)
			SetAIState(EAIState::Chase);
	}
	else
	{
		DetectionValue -= DetectionDecreaseValue * DeltaSeconds;
		if (DetectionValue <= 0.0f)
			SetAIState(EAIState::Patrol);
	}
}

void AProjectRAIController::UpdateChase()
{
	auto* MyPawn = GetPawn<AProjectRCharacter>();

	if (!TargetActor)
	{
		UBuffLibrary::ReleaseBuff<ULock>(MyPawn);
		UBuffLibrary::ApplyBuff<URun>(MyPawn);
	}
	else
	{
		const FVector PawnLocation = MyPawn->GetActorLocation();
		const FVector TargetLocation = TargetActor->GetActorLocation();
		const float DistanceSquared = (PawnLocation - TargetLocation).SizeSquared();

		if (DistanceSquared <= LockRadiusSquared)
		{
			UBuffLibrary::ReleaseBuff<URun>(MyPawn);
			UBuffLibrary::ApplyBuff<ULock>(MyPawn);
		}
		else
		{
			UBuffLibrary::ReleaseBuff<ULock>(MyPawn);
			UBuffLibrary::ApplyBuff<URun>(MyPawn);
		}
	}
}

void AProjectRAIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
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

void AProjectRAIController::OnSightUpdated(AActor* Actor, const FAIStimulus& Stimulus)
{
	if (Cast<AProjectRCharacter>(Actor)->IsDeath())
		return;

	bIsSeePlayer = Stimulus.IsActive();

	auto* MyBlackboard = GetBlackboardComponent();
	if (MyBlackboard)
		MyBlackboard->SetValueAsBool(TEXT("IsSeePlayer"), bIsSeePlayer);

	if (bIsSeePlayer)
	{
		TargetActor = Actor;
		
		if (auto* Lock = UBuffLibrary::GetBuff<ULock>(GetPawn<AProjectRCharacter>()))
			Lock->SetLockTarget(TargetActor);

		if (AIState == EAIState::Patrol)
			SetAIState(EAIState::Detection);
	}
	else
		TargetActor = nullptr;

	if (MyBlackboard)
		MyBlackboard->SetValueAsObject(TEXT("TargetActor"), TargetActor);
}

void AProjectRAIController::SetFloorLocation(const FVector& BaseLocation)
{
	if (auto* MyBlackboard = GetBlackboardComponent())
	{
		FVector End = BaseLocation;
		End.Z = -1000.0f;

		FHitResult Hit;
		GetWorld()->LineTraceSingleByObjectType(Hit, BaseLocation, End, { ECollisionChannel::ECC_WorldStatic });
		MyBlackboard->SetValueAsVector(TEXT("TargetLocation"), Hit.ImpactPoint);
	}
}

void AProjectRAIController::OnDeath(AController* LastInstigator)
{
	UnPossess();
}
