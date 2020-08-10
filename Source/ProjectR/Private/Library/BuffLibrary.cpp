// Fill out your copyright notice in the Description page of Project Settings.

#include "Library/BuffLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Buff/Buff.h"
#include "Component/StatComponent.h"

UBuff* UBuffLibrary::GetBuff(AActor* Target, TSubclassOf<UBuff> BuffClass)
{
	if (!Target) return nullptr;
	auto* StatComp = Cast<UStatComponent>(Target->GetComponentByClass(UStatComponent::StaticClass()));
	return StatComp ? StatComp->GetBuff(BuffClass) : nullptr;
}

const UBuff* UBuffLibrary::GetBuff(const AActor* Target, TSubclassOf<UBuff> BuffClass)
{
	return GetBuff(const_cast<AActor*>(Target), BuffClass);
}

void UBuffLibrary::ApplyBuff(AActor* Target, TSubclassOf<UBuff> BuffClass)
{
	if (UBuff* Buff = GetBuff(Target, BuffClass))
		Buff->Apply();
}

void UBuffLibrary::ReleaseBuff(AActor* Target, TSubclassOf<UBuff> BuffClass)
{
	if (UBuff* Buff = GetBuff(Target, BuffClass))
		Buff->Release();
}

void UBuffLibrary::ApplyBuffWithDuration(AActor* Target, TSubclassOf<UBuff> BuffClass, float Duration)
{
	UBuff* Buff = GetBuff(Target, BuffClass);
	if (!Buff) return;

	Buff->Apply();

	FTimerHandle Handle;
	Target->GetWorldTimerManager().SetTimer(Handle, [Buff]
		{ Buff->Release(); }, Duration, false);
}

bool UBuffLibrary::IsActivate(const AActor* Target, TSubclassOf<UBuff> BuffClass)
{
	const UBuff* Buff = GetBuff(Target, BuffClass);
	return Buff ? Buff->IsActivate() : false;
}
