// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Lock.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ProjectRCharacter.h"

void ULock::Lock(AActor* InLockedTarget)
{
	if (IsBlocked()) return;

	LockedTarget = InLockedTarget;
	Apply();
}

void ULock::Tick(float DeltaSeconds)
{
	if (!LockedTarget) return;

	const FVector TargetLocation = LockedTarget->GetActorLocation();

	const FRotator ControlLookAt = UKismetMathLibrary::
		FindLookAtRotation(GetTarget()->GetViewLocation(), TargetLocation);

	GetTarget()->GetController()->SetControlRotation(ControlLookAt);
}

void ULock::OnApply()
{
	GetTarget()->GetCharacterMovement()->bUseControllerDesiredRotation = LockedTarget != nullptr;
	GetTarget()->GetCharacterMovement()->bOrientRotationToMovement = false;
	bIsLocked = true;
}

void ULock::OnRelease()
{
	GetTarget()->GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetTarget()->GetCharacterMovement()->bOrientRotationToMovement = true;
	bIsLocked = false;
}

bool ULock::IsActivate_Implementation() const
{
	return bIsLocked;
}
