// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/PRMovementComponent.h"

void UPRMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	Super::AddInputVector(WorldVector, bForce);
	InputVector += WorldVector;
}

FVector UPRMovementComponent::ConsumeInputVector()
{
	LastInputVector = InputVector;
	InputVector = FVector::ZeroVector;
	return Super::ConsumeInputVector();
}