// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Curves/CurveFloat.h"
#include "NavigationInvokerComponent.h"
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
	bSetControlRotationFromPawnOrientation = false;

	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));
	
	auto* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISenseConfig_Sight"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	GetPerceptionComponent()->ConfigureSense(*SightConfig);

	NavigationInvoker = CreateDefaultSubobject<UNavigationInvokerComponent>(TEXT("Navigation Invoker"));
	NavigationInvoker->SetGenerationRadii(7000.0f, 8000.0f);
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

	SightRadius = LogicData.SightRadius;
	LockRadiusSquared = FMath::Square(LogicData.LockRadius);

	auto* SightConfig = Cast<UAISenseConfig_Sight>(GetPerceptionComponent()
		->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>()));

	SightConfig->SightRadius = LogicData.SightRadius;
	SightConfig->LoseSightRadius = LogicData.LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = LogicData.FOV;
	GetPerceptionComponent()->RequestStimuliListenerUpdate();
}

void AProjectRAIController::BeginPlay()
{
	Super::BeginPlay();
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &AProjectRAIController::OnPerceptionUpdated);
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

void AProjectRAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{

}

void AProjectRAIController::OnDeath(AController* LastInstigator)
{
	UnPossess();
}
