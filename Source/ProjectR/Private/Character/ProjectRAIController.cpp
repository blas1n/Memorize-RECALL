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
