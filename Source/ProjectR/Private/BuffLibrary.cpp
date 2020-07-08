// Fill out your copyright notice in the Description page of Project Settings.

#include "BuffLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Buff/Buff.h"
#include "Character/ProjectRCharacter.h"
#include "Character/ProjectRPlayerState.h"

UBuff* UBuffLibrary::GetBuff(AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass)
{
	auto* PlayerState = Character->GetPlayerState<AProjectRPlayerState>();
	return PlayerState ? PlayerState->GetBuff(BuffClass) : nullptr;
}

const UBuff* UBuffLibrary::GetBuff(const AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass)
{
	auto* PlayerState = Character->GetPlayerState<AProjectRPlayerState>();
	return PlayerState ? PlayerState->GetBuff(BuffClass) : nullptr;
}

void UBuffLibrary::ApplyBuff(AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass)
{
	if (UBuff* Buff = GetBuff(Character, BuffClass))
		Buff->Apply();
}

void UBuffLibrary::ReleaseBuff(AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass)
{
	if (UBuff* Buff = GetBuff(Character, BuffClass))
		Buff->Release();
}

void UBuffLibrary::ApplyBuffWithDuration(AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass, float Duration)
{
	UBuff* Buff = GetBuff(Character, BuffClass);
	if (!Buff) return;

	Buff->Apply();

	FTimerHandle Handle;
	Character->GetWorldTimerManager().SetTimer(Handle, [Buff]
		{ Buff->Release(); }, Duration, false);
}

void UBuffLibrary::BlockBuff(AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass)
{
	if (UBuff* Buff = GetBuff(Character, BuffClass))
		Buff->Block();
}

void UBuffLibrary::UnblockBuff(AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass)
{
	if (UBuff* Buff = GetBuff(Character, BuffClass))
		Buff->Unblock();
}

bool UBuffLibrary::IsActivate(const AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass)
{
	const UBuff* Buff = GetBuff(Character, BuffClass);
	return Buff ? Buff->IsActivate() : false;
}

bool UBuffLibrary::IsBlocked(const AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass)
{
	const UBuff* Buff = GetBuff(Character, BuffClass);
	return Buff ? Buff->IsBlocked() : false;
}
