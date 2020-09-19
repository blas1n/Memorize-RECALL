// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/Door.h"
#include "Engine/TriggerBase.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Framework/PRPlayerController.h"

ADoor::ADoor()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ADoor::Open()
{
	Direction = 1.0f;
	SetTick();
	OnOpen.Broadcast();
}

void ADoor::Close()
{
	Direction = -1.0f;
	SetTick();
	OnClose.Broadcast();
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	if (OpenTrigger)
	{
		OpenTrigger->OnActorBeginOverlap.AddDynamic(this, &ADoor::OnOpenTriggerBeginOverlap);
		OpenTrigger->OnActorEndOverlap.AddDynamic(this, &ADoor::OnOpenTriggerEndOverlap);
	}

	if (CloseTrigger)
		CloseTrigger->OnActorBeginOverlap.AddDynamic(this, &ADoor::OnCloseTriggerBeginOverlap);
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AddActorLocalOffset(FVector{ 0.0f, 0.0f, DeltaTime * Speed * Direction });
}

void ADoor::SetOpenTrigger(ATriggerBase* NewOpenTrigger)
{
	OpenTrigger = NewOpenTrigger;
	OpenTrigger->OnActorBeginOverlap.AddDynamic(this, &ADoor::OnOpenTriggerBeginOverlap);
	OpenTrigger->OnActorEndOverlap.AddDynamic(this, &ADoor::OnOpenTriggerEndOverlap);
}

void ADoor::SetCloseTrigger(ATriggerBase* NewCloseTrigger)
{
	CloseTrigger = NewCloseTrigger;
	CloseTrigger->OnActorBeginOverlap.AddDynamic(this, &ADoor::OnCloseTriggerBeginOverlap);
}

void ADoor::Interact_Implementation(APRCharacter* Target)
{
	Open();
	OpenTrigger->Destroy();
}

void ADoor::OnOpenTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	auto* Controller = Cast<APawn>(OtherActor)->GetController<APRPlayerController>();
	if (Controller) Controller->RegisterInteractor(this);
}

void ADoor::OnOpenTriggerEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	auto* Controller = Cast<APawn>(OtherActor)->GetController<APRPlayerController>();
	if (Controller) Controller->UnregisterInteractor();
}

void ADoor::OnCloseTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (OtherActor != Player) return;
	
	Close();
	CloseTrigger->Destroy();
}

void ADoor::SetTick()
{
	SetActorTickEnabled(true);
	GetWorldTimerManager().SetTimer(Timer, [this]
	{
		SetActorTickEnabled(false);
	}, Duration, false);
}
