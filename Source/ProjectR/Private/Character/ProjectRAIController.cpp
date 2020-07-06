// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ProjectRCharacter.h"

AProjectRAIController::AProjectRAIController()
	: Super()
{
	bWantsPlayerState = true;
}

void AProjectRAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	User = Cast<AProjectRCharacter>(InPawn);
}

void AProjectRAIController::OnUnPossess()
{
	User = nullptr;
	Super::OnUnPossess();
}

void AProjectRAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!User) return;

	FVector Target;
	if (!GetMoveTargetLocation(Target)) return;

	const FRotator Look = UKismetMathLibrary::FindLookAtRotation(User->GetActorLocation(), Target);
	User->SetTurn(Look.Yaw);
}

bool AProjectRAIController::GetMoveTargetLocation(FVector& Out) const
{
	auto* MyBlackbaord = GetBlackboardComponent();
	if (!MyBlackbaord) return false;

	Out = MyBlackbaord->GetValueAsVector(TEXT("TargetLocation"));
	return true;
}
