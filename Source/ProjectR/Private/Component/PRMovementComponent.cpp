// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/PRMovementComponent.h"
#include "Net/UnrealNetwork.h"

void UPRMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	Super::AddInputVector(WorldVector, bForce);
	InputVector += WorldVector;
}

FVector UPRMovementComponent::ConsumeInputVector()
{
	if (GetOwnerRole() == ROLE_AutonomousProxy)
	{
		LastInputVector = InputVector;
		InputVector = FVector::ZeroVector;
		ServerSetLastInputVector(LastInputVector);
	}

	return Super::ConsumeInputVector();
}