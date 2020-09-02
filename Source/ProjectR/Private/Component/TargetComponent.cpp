// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/TargetComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AIPerceptionComponent.h"

UTargetComponent::UTargetComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTargetComponent::Initialize(UAIPerceptionComponent* Perception)
{
	Perception->OnTargetPerceptionUpdated.AddDynamic(this, &UTargetComponent::OnPerceptionUpdated);
}

void UTargetComponent::SetInterval(float InInterval)
{
	ServerSetInterval(InInterval);
}

void UTargetComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ENetRole::ROLE_Authority)
		SetComponentTickInterval(Interval);
	else
		SetComponentTickEnabled(false);
}

void UTargetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetOwnerRole() != ENetRole::ROLE_Authority)
		return;
	
	const int32 Num = TargetActors.Num();
	if (Num == 0)
	{
		TargetedActor = nullptr;
		return;
	}

	auto* MyOwner = GetOwner();
	const FVector Loc = MyOwner->GetActorLocation();
	FRotator Rot = MyOwner->GetActorRotation();

	if (auto* Pawn = Cast<APawn>(MyOwner))
		if (Pawn->IsPawnControlled())
			Rot = Pawn->GetControlRotation();
	
	TArray<float> RotationArray;
	RotationArray.Init(0.0f, Num);

	int32 MinIdx = 0;

	for (int32 Idx = 0; Idx < Num; ++Idx)
	{
		const FRotator Look = UKismetMathLibrary
			::FindLookAtRotation(Loc, TargetActors[Idx]->GetActorLocation());
		
		RotationArray[Idx] = FMath::Abs((Look - Rot).Yaw);
		if (RotationArray[Idx] < RotationArray[MinIdx])
			MinIdx = Idx;
	}

	TargetedActor = TargetActors[MinIdx];
}

void UTargetComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UTargetComponent, TargetedActor);
	DOREPLIFETIME(UTargetComponent, Interval);
}

void UTargetComponent::ServerSetInterval_Implementation(float InInterval)
{
	Interval = InInterval;
	SetComponentTickInterval(Interval);
}

void UTargetComponent::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.IsActive())
		TargetActors.AddUnique(Actor);
	else
		TargetActors.Remove(Actor);
}
