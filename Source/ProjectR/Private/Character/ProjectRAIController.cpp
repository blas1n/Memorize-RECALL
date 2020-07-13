// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Character/ProjectRCharacter.h"

AProjectRAIController::AProjectRAIController()
	: Super()
{
	bWantsPlayerState = true;

	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
}

void AProjectRAIController::InitPerception(const FPerceptionData& Data)
{
	SightConfig->SightRadius = Data.SightRadius;
	SightConfig->LoseSightRadius = Data.LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = Data.FOV;
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

void AProjectRAIController::OnDeath(AController* LastInstigator)
{
	UnPossess();
}
