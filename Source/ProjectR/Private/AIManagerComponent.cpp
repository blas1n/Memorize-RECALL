// Fill out your copyright notice in the Description page of Project Settings.

#include "AIManagerComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ProjectRAIController.h"
#include "Character/ProjectRCharacter.h"
#include "Data/AIData.h"
#include "Framework/ProjectRGameInstance.h"
#include "ProjectRStatics.h"

const TArray<FName>& UAIManagerComponent::GetWeaponNames()
{
	if (!bIsInit)
		Initialize();

	return WeaponNames;
}

void UAIManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!bIsInit)
		Initialize();
}

void UAIManagerComponent::Initialize()
{
	bIsInit = true;

	const auto* GameInstance = Cast<UProjectRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	const auto* DataTable = GameInstance->GetDataTable(TEXT("AIData"));
	const auto* MyPawn = Cast<AProjectRCharacter>(GetOwner());
	const auto& AIData = *DataTable->FindRow<FAIData>(MyPawn->GetName(), "", false);

	WeaponNames = AIData.WeaponNames;
	
	UProjectRStatics::AsyncLoad(AIData.Mesh, [this, &MeshPtr = AIData.Mesh, &AnimClass = AIData.AnimClass]() mutable
		{
			auto* MeshComp = Cast<AProjectRCharacter>(GetOwner())->GetMesh();
			MeshComp->SetSkeletalMesh(MeshPtr.Get());
			MeshComp->SetAnimClass(AnimClass);
		});
	
	auto* Controller = MyPawn->GetController<AProjectRAIController>();
	Controller->InitLogic(AIData.LogicData);
}
