// Fill out your copyright notice in the Description page of Project Settings.

#include "AiSkill.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectRCharacter.h"
#include "Weapon.h"

AAiSkill::AAiSkill()
	: Super()
{
	Player = nullptr;
	Range = 0.0f;
}

bool AAiSkill::IsInRange() const
{
	if (!IsValid(Player) || !IsValid(GetUser()))
		return false;

	FVector PlayerLocation = Player->GetActorLocation();
	FVector UserLocation = GetUser()->GetActorLocation();

	float SizeSqrt = (PlayerLocation - UserLocation).SizeSquared();
	return SizeSqrt <= FMath::Square(Range);
}

void AAiSkill::BeginPlay()
{
	Super::BeginPlay();
	Player = UGameplayStatics::GetPlayerPawn(Cast<UObject>(GetWorld()), 0);
}