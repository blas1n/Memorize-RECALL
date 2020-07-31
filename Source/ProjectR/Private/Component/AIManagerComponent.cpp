// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/AIManagerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Character/ProjectRAIController.h"
#include "Character/ProjectRCharacter.h"
#include "Data/LogicData.h"
#include "Framework/ProjectRGameInstance.h"
#include "Library/ProjectRStatics.h"

UAIManagerComponent::UAIManagerComponent()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_LogicData.DT_LogicData'"));
	LogicDataTable = DataTable.Object;
	Initialize();
}

void UAIManagerComponent::SetTeamID(const FGenericTeamId& NewTeamID)
{
	if (TeamID == NewTeamID) return;

	TeamID = NewTeamID;
	ApplyTeamID();
}

#if WITH_EDITOR

void UAIManagerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Initialize();
}

#endif

void UAIManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
	ApplyTeamID();
}

void UAIManagerComponent::Initialize()
{
	const auto* Data = LogicDataTable->FindRow<FLogicData>(FName{ *FString::FromInt(LogicKey) }, TEXT(""));
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found logic data %d!"), LogicKey);
		return;
	}
	
	auto* MyOwner = Cast<APawn>(GetOwner());
	if (!MyOwner) return;
	
	auto* Controller = MyOwner->GetController<AProjectRAIController>();
	if (!Controller) return;

	Controller->InitLogic(BehaviorTree, *Data);
}

void UAIManagerComponent::ApplyTeamID()
{
	auto* MyOwner = Cast<APawn>(GetOwner());
	if (!MyOwner) return;

	auto* Controller = MyOwner->GetController<IGenericTeamAgentInterface>();
	if (!Controller) return;

	Controller->SetGenericTeamId(TeamID);
}
