// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Lock.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/ProjectRCharacter.h"

void ULock::Tick(float DeltaSeconds)
{
	if (!bIsLocked || !LockedTarget)
		return;

	FRotator LookRotation = UKismetMathLibrary::
		FindLookAtRotation(GetTarget()->GetViewLocation(), LockedTarget->GetActorLocation());

	const FRotator NowRoation = GetTarget()->GetControlRotation();
	const FRotator NowRotation = FMath::Lerp(NowRoation, LookRotation, DeltaSeconds * 5.0f);
	GetTarget()->GetController()->SetControlRotation(NowRotation);
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
