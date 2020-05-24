// Fill out your copyright notice in the Description page of Project Settings.

#include "Buff.h"

ABuff::ABuff()
	: Super()
{
	PrimaryActorTick.bCanEverTick = false;
	Target = nullptr;
}

void ABuff::ApplyBuff(AProjectRCharacter* InTarget, float Duration)
{
	Target = InTarget;
	SetLifeSpan(Duration);
}
