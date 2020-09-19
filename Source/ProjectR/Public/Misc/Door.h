// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/Interactable.h"
#include "Door.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnOpen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClose);

UCLASS()
class PROJECTR_API ADoor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:
	ADoor();

	UFUNCTION(BlueprintCallable)
	void Open();

	UFUNCTION(BlueprintCallable)
	void Close();

private:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintSetter)
	void SetOpenTrigger(class ATriggerBase* NewOpenTrigger);

	UFUNCTION(BlueprintSetter)
	void SetCloseTrigger(ATriggerBase* NewCloseTrigger);

	void Interact_Implementation(APRCharacter* Target) override;

	UFUNCTION()
	void OnOpenTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnOpenTriggerEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnCloseTriggerBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void SetTick();

public:
	UPROPERTY(BlueprintAssignable)
	FOnOpen OnOpen;

	UPROPERTY(BlueprintAssignable)
	FOnClose OnClose;

private:
	UPROPERTY(EditAnywhere, BlueprintSetter = SetOpenTrigger, meta = (AllowPrivateAccess = true))
	ATriggerBase* OpenTrigger;

	UPROPERTY(EditAnywhere, BlueprintSetter = SetCloseTrigger, meta = (AllowPrivateAccess = true))
	ATriggerBase* CloseTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	float Duration;

	float Direction;

	FTimerHandle Timer;
};
