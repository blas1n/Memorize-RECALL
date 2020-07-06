// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Lock.h"
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

	const FVector ActorLocation = GetTarget()->GetActorLocation();
	
	FRotator ActorLookAt = UKismetMathLibrary::
		FindLookAtRotation(GetTarget()->GetActorLocation(), TargetLocation);
	
	ActorLookAt.Pitch = 0.0f;
	
	const FRotator ActorRotation = GetTarget()->GetActorRotation();
	ActorLookAt = FMath::Lerp(ActorRotation, ActorLookAt, DeltaSeconds * 5.0f);
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
