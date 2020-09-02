// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "TargetComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTR_API UTargetComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UTargetComponent();
	
	void Initialize(class UAIPerceptionComponent* Perception);

	UFUNCTION(BlueprintSetter)
	void SetInterval(float InInterval);

	FORCEINLINE AActor* GetTargetedActor() const noexcept { return TargetedActor; }

private:
	void BeginPlay() override;
	void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetInterval(float InInterval);

	void ServerSetInterval_Implementation(float InInterval);
	FORCEINLINE bool ServerSetInterval_Validate(float InInterval) { return InInterval > 0.0f; }

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	UPROPERTY(Replicated, Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TArray<AActor*> TargetActors;

	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	AActor* TargetedActor;

	UPROPERTY(Replicated, EditAnywhere, BlueprintSetter = SetInterval, meta = (AllowPrivateAccess = true))
	float Interval;
};
