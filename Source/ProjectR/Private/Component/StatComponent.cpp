// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/StatComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Data/StatData.h"
#include "Framework/PRCharacter.h"

UStatComponent::UStatComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_StatData.DT_StatData'"));
	StatDataTable = DataTable.Object;
}

void UStatComponent::SetLevel(uint8 NewLevel)
{
	ServerSetLevel(NewLevel);
}

void UStatComponent::Heal(float Value)
{
	check(GetOwnerRole() == ENetRole::ROLE_Authority);
	Health = FMath::Clamp(Health + Value, 0.0f, MaxHealth);
}

void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwnerRole() == ENetRole::ROLE_Authority)
		SetLevel(Level);
	else
		OnRep_Level();
}

void UStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UStatComponent, Level);
	DOREPLIFETIME(UStatComponent, Health);
}

void UStatComponent::ServerSetLevel_Implementation(uint8 NewLevel)
{
	Level = NewLevel;

	const float BeforeMaxHealth = MaxHealth;
	OnRep_Level();

	Health += MaxHealth - BeforeMaxHealth;
	if (Health < 0.0f) Health = 0.0f;
}

void UStatComponent::OnRep_Level()
{
	const FString Key = FString::FromInt(StatKey) + FString::FromInt(Level);
	const auto* Data = StatDataTable->FindRow<FStatData>(FName{ *Key }, TEXT(""), false);
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found stat data %s!"), *Key);
		return;
	}

	MaxHealth = Data->MaxHealth;
	RunSpeed = Data->RunSpeed;
	WalkSpeed = Data->WalkSpeed;
	LockSpeed = Data->LockSpeed;
	
	const auto* MyOwner = Cast<APRCharacter>(GetOwner());
	const EMoveState MoveState = MyOwner->GetMoveState();
	float Speed = WalkSpeed;

	if (MoveState == EMoveState::Run)
		Speed = RunSpeed;
	else if (MyOwner->IsLocked())
		Speed = LockSpeed;

	MyOwner->GetCharacterMovement()->MaxWalkSpeed = Speed;
	OnChangedLevel.Broadcast(Level);
}
