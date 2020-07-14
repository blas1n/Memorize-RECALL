// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/AssetManager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Buff/Lock.h"
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

void AProjectRAIController::InitLogic(const FLogicData& LogicData)
{
	UProjectRStatics::AsyncLoad(LogicData.BehaviorTree, [this, &LogicData]() mutable
		{
			RunBehaviorTree(LogicData.BehaviorTree.Get());
			InitBlackboard(LogicData);
		});

	DetectionIncrease = LogicData.DetectionIncrease;
	DetectionDecrease = LogicData.DetectionDecrease;

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

	if (AIState == EAIState::Patrol || AIState == EAIState::Quest)
	{
		if (AIState == EAIState::Quest)
			DetectionValue = 0.0f;

		UBuffLibrary::ReleaseBuff<ULock>(GetPawn<AProjectRCharacter>());
		return;
	}

	UBuffLibrary::GetBuff<ULock>(GetPawn<AProjectRCharacter>())->Lock(TargetActor);
}

void AProjectRAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AIState == EAIState::Chase)
		SetFloorLocation(TargetActor->GetActorLocation());

	if (AIState != EAIState::Detection)
		return;
	
	if (bIsSeePlayer)
	{
		DetectionValue += DetectionIncrease * DeltaSeconds;
		if (DetectionValue >= 1.0f)
		{
			SetAIState(EAIState::Chase);
			DetectionValue = 1.0f;
		}
	}
	else
	{
		DetectionValue -= DetectionDecrease * DeltaSeconds;
		if (DetectionValue <= 0.0f)
		{
			SetAIState(EAIState::Patrol);
			DetectionValue = 0.0f;
		}
	}
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
	auto* MyBlackboard = GetBlackboardComponent();
	MyBlackboard->SetValueAsVector(TEXT("HomeLocation"), GetPawn()->GetActorLocation());
	MyBlackboard->SetValueAsFloat(TEXT("PatrolRadius"), LogicData.PatrolRadius);
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
	bIsSeePlayer = Stimulus.IsActive();
	if (auto* MyBlackboard = GetBlackboardComponent())
		MyBlackboard->SetValueAsBool(TEXT("IsSeePlayer"), bIsSeePlayer);

	if (bIsSeePlayer)
	{
		TargetActor = Actor;

		if (AIState == EAIState::Patrol)
			SetAIState(EAIState::Detection);
		else if (AIState == EAIState::Quest)
			SetAIState(EAIState::Chase);
	}
	else
	{
		TargetActor = nullptr;

		if (AIState == EAIState::Detection)
			SetFloorLocation(Stimulus.StimulusLocation);
		else if (AIState == EAIState::Chase)
			SetAIState(EAIState::Quest);
	}
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
