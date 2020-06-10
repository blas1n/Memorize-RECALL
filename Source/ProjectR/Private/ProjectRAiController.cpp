// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectRAiController.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "PlayerCharacter.h"

AProjectRAiController::AProjectRAiController()
	: Super()
{
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));
	bIsInit = false;
}

void AProjectRAiController::RunAi(const FAiData& Data)
{
	check(!bIsInit);
	bIsInit = true;

	SightConfig = NewObject<UAISenseConfig_Sight>(this, TEXT("Sight Config"));

	SightConfig->SightRadius = Data.SightRadius;
	SightConfig->LoseSightRadius = Data.LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = Data.FOV;
	SightConfig->SetMaxAge(Data.SightAge);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	GetPerceptionComponent()->ConfigureSense(*SightConfig);

	RunBehaviorTree(Data.BehaviorTree);
	SetBlackboard(GetBlackboardComponent(), Data);

	AProjectRCharacter* MyPawn = GetPawn<AProjectRCharacter>();
	MyPawn->OnDeath.AddDynamic(this, &AProjectRAiController::Death);
}

void AProjectRAiController::BeginPlay()
{
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	Super::BeginPlay();
}

void AProjectRAiController::Death(AController* Attacker)
{
	StopMovement();
	UnPossess();
	Destroy();
}