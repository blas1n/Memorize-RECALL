// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponSelector.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Character/ProjectRCharacter.h"
#include "Component/WeaponComponent.h"
#include "Data/WeaponData.h"
#include "Library/ProjectRStatics.h"

AWeaponSelector::AWeaponSelector()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_WeaponData.DT_WeaponData'"));
	WeaponDataTable = DataTable.Object;

	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = Box;

	RightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Mesh"));
	RightMesh->SetupAttachment(RootComponent);

	LeftMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Mesh"));
	LeftMesh->SetupAttachment(RootComponent);

	Initialize();
}

#if WITH_EDITOR

void AWeaponSelector::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property && PropertyChangedEvent.GetPropertyName() == TEXT("WeaponKey"))
		Initialize();
}

#endif

void AWeaponSelector::Initialize()
{
	const auto* Data = WeaponDataTable->FindRow<FWeaponData>(FName{ *FString::FromInt(WeaponKey) }, TEXT(""));
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found weapon data %d!"), WeaponKey);
		return;
	}

	if (!Data->RightMesh.IsNull())
	{
		UProjectRStatics::AsyncLoad(Data->RightMesh, [this, &RightMeshPtr = Data->RightMesh]() mutable
		{
			RightMesh->SetStaticMesh(RightMeshPtr.Get());
		});
	}
	else
	{
		RightMesh->SetStaticMesh(nullptr);
	}

	if (!Data->LeftMesh.IsNull())
	{
		UProjectRStatics::AsyncLoad(Data->LeftMesh, [this, &LeftMeshPtr = Data->LeftMesh]() mutable
		{
			LeftMesh->SetStaticMesh(LeftMeshPtr.Get());
		});
	}
	else
	{
		LeftMesh->SetStaticMesh(nullptr);
	}
}

void AWeaponSelector::BeginPlay()
{
	Super::BeginPlay();

	Initialize();
	Box->OnComponentBeginOverlap.AddDynamic(this, &AWeaponSelector::OnSelect);
}

void AWeaponSelector::OnSelect(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		return;

	auto* Player = Cast<AProjectRCharacter>(OtherActor);
	Player->AddWeapon(WeaponKey);
	Destroy();
}
