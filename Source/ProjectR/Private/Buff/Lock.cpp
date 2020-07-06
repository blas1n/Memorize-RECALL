// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Lock.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ProjectRCharacter.h"

void ULock::Tick(float DeltaSeconds)
{
	if (!LockedTarget) return;

	const FVector ControlLocation = GetTarget()->GetViewLocation();
	const FRotator ControlRotation = GetTarget()->GetControlRotation();
	const FRotator ControlLookAt = GetLookAtToTarget(ControlLocation, ControlRotation, DeltaSeconds);
	GetTarget()->GetController()->SetControlRotation(ControlLookAt);

	const FVector ActorLocation = GetTarget()->GetActorLocation();
	const FRotator ActorRotation = GetTarget()->GetActorRotation();

	FRotator ActorLookAt = GetLookAtToTarget(ActorLocation, ActorRotation, DeltaSeconds);
	ActorLookAt.Pitch = 0.0f;
	
	GetTarget()->SetActorRotation(ActorLookAt);
}

void ULock::BeginBuff()
{
	GetTarget()->Walk();
	bIsLocked = true;
}

void ULock::EndBuff()
{
	bIsLocked = false;
}

bool ULock::IsActivate_Implementation() const
{
	return bIsLocked;
}

FRotator ULock::GetLookAtToTarget(const FVector& Location, const FRotator& Rotation, float DeltaSeconds)
{
	const FVector TargetLocation = LockedTarget->GetActorLocation();
	const FRotator LookRotation = UKismetMathLibrary::
		FindLookAtRotation(Location, TargetLocation);

	return FMath::Lerp(Rotation, LookRotation, DeltaSeconds * 5.0f);
}
