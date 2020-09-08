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
	if (GetOwner()->HasLocalNetOwner())
	{
		LastInputVector = InputVector;
		InputVector = FVector::ZeroVector;
	}

	if (GetOwnerRole() == ROLE_AutonomousProxy)
		ServerSetLastInputVector(LastInputVector);

	return Super::ConsumeInputVector();
}