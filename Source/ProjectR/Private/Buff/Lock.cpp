// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Lock.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ProjectRCharacter.h"
#include "Character/ProjectRPlayerState.h"

void ULock::SetLockTarget(AActor* InLockedTarget)
{
	LockedTarget = InLockedTarget;
}

void ULock::Tick(float DeltaSeconds)
{
	if (!LockedTarget) return;

	const FVector TargetLocation = LockedTarget->GetActorLocation();

	FVector Loc; FRotator Rot;
	GetTarget()->GetActorEyesViewPoint(Loc, Rot);

	const FRotator ControlLookAt = UKismetMathLibrary::
		FindLookAtRotation(Loc, TargetLocation);

	if (AController* Controller = GetTarget()->GetController())
		Controller->SetControlRotation(ControlLookAt);
}

void ULock::OnApply()
{
	bIsLocked = true;
}

void ULock::OnRelease()
{
	bIsLocked = false;
}

bool ULock::IsActivate_Implementation() const
{
	return bIsLocked;
}
