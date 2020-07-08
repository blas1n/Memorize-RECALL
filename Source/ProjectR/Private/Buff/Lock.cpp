// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Lock.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ProjectRCharacter.h"

void ULock::Tick(float DeltaSeconds)
{
	if (!LockedTarget) return;

	const FVector TargetLocation = LockedTarget->GetActorLocation();

	const FRotator ControlLookAt = UKismetMathLibrary::
		FindLookAtRotation(GetTarget()->GetViewLocation(), TargetLocation);

	GetTarget()->GetController()->SetControlRotation(ControlLookAt);

	if (GetTarget()->GetCharacterMovement()->Velocity.SizeSquared2D() <= 10000.0f)
		return;

	const FVector ActorLocation = GetTarget()->GetActorLocation();
	
	FRotator ActorLookAt = UKismetMathLibrary::
		FindLookAtRotation(GetTarget()->GetActorLocation(), TargetLocation);
	
	ActorLookAt.Pitch = 0.0f;

	const FRotator ActorRotation = GetTarget()->GetActorRotation();
	ActorLookAt = FMath::Lerp(ActorRotation, ActorLookAt, DeltaSeconds * 5.0f);
	GetTarget()->SetActorRotation(ActorLookAt);
}

void ULock::OnApply()
{
	GetTarget()->GetCharacterMovement()->bOrientRotationToMovement = false;
	bIsLocked = true;
}

void ULock::OnRelease()
{
	GetTarget()->GetCharacterMovement()->bOrientRotationToMovement = true;
	bIsLocked = false;
}

bool ULock::IsActivate_Implementation() const
{
	return bIsLocked;
}
