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
	
	if (IsActivate())
	{
		GetTarget()->GetCharacterMovement()->
			bUseControllerDesiredRotation = LockedTarget != nullptr;
	}
}

void ULock::Tick(float DeltaSeconds)
{
	if (!LockedTarget) return;

	const FVector TargetLocation = LockedTarget->GetActorLocation();

	const FRotator ControlLookAt = UKismetMathLibrary::
		FindLookAtRotation(GetTarget()->GetLookLocation(), TargetLocation);

	if (AController* Controller = GetTarget()->GetController())
		Controller->SetControlRotation(ControlLookAt);
}

void ULock::OnApply()
{
	bIsLocked = true;

	auto* Movement = GetTarget()->GetCharacterMovement();
	bWasOrientMovement = Movement->bOrientRotationToMovement;
	bWasDesiredRotation = Movement->bUseControllerDesiredRotation;
	Movement->bOrientRotationToMovement = false;
	Movement->bUseControllerDesiredRotation = LockedTarget != nullptr;

	if (auto* PlayerState = GetTarget()->GetPlayerState<AProjectRPlayerState>())
		GetTarget()->GetCharacterMovement()->MaxWalkSpeed = PlayerState->GetLockSpeed();
	
	bIsLocked = true;
}

void ULock::OnRelease()
{
	auto* Movement = GetTarget()->GetCharacterMovement();
	Movement->bOrientRotationToMovement = bWasOrientMovement;
	Movement->bUseControllerDesiredRotation = bWasDesiredRotation;

	if (auto* PlayerState = GetTarget()->GetPlayerState<AProjectRPlayerState>())
		GetTarget()->GetCharacterMovement()->MaxWalkSpeed = PlayerState->GetWalkSpeed();

	bIsLocked = false;
}

bool ULock::IsActivate_Implementation() const
{
	return bIsLocked;
}
