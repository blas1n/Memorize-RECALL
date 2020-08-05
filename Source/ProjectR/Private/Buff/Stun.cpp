// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff/Stun.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/PlayerController.h"
#include "Buff/Lock.h"
#include "Framework/PRCharacter.h"
#include "Library/BuffLibrary.h"

void UStun::OnApply()
{
	AController* Controller = GetTarget()->GetController();
	if (++Count > 1 || !Controller || GetTarget()->GetRemoteRole() != ENetRole::ROLE_AutonomousProxy)
		return;
	
	if (GetTarget()->IsPlayerControlled())
		Cast<APlayerController>(Controller)->DisableInput(nullptr);
	else if (auto* Brain = Cast<AAIController>(Controller)->GetBrainComponent())
		Brain->StopLogic(TEXT("Stun"));
}

void UStun::OnRelease()
{
	AController* Controller = GetTarget()->GetController();
	if (--Count > 0 || !Controller || GetTarget()->GetRemoteRole() != ENetRole::ROLE_AutonomousProxy)
		return;

	if (GetTarget()->IsPlayerControlled())
		Cast<APlayerController>(Controller)->EnableInput(nullptr);
	else if (auto* Brain = Cast<AAIController>(Controller)->GetBrainComponent())
		Brain->RestartLogic();
}

bool UStun::IsActivate_Implementation() const
{
	return Count > 0;
}
