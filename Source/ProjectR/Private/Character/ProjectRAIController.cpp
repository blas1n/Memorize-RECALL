// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ProjectRCharacter.h"

AProjectRAIController::AProjectRAIController()
	: Super()
{
	bWantsPlayerState = true;
void AProjectRAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	Cast<AProjectRCharacter>(InPawn)->OnDeath
		.AddDynamic(this, &AProjectRAIController::OnDeath);
}

void AProjectRAIController::OnUnPossess()
{
	Cast<AProjectRCharacter>(GetPawn())->OnDeath
		.RemoveDynamic(this, &AProjectRAIController::OnDeath);

	Super::OnUnPossess();
}

void AProjectRAIController::OnDeath(AController* LastInstigator)
{
	UnPossess();
}
