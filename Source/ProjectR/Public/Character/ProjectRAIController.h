// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Data/PerceptionData.h"
#include "ProjectRAIController.generated.h"

UCLASS()
class PROJECTR_API AProjectRAIController final : public AAIController
{
	GENERATED_BODY()

public:
	AProjectRAIController();

protected:
	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void InitPerception(const struct FPerceptionData& Data);

private:
	void OnPossess(APawn* InPawn) override;
	void OnUnPossess() override;

	UFUNCTION()
	void OnDeath(AController* LastInstigator);

private:
	UPROPERTY()
	class UAISenseConfig_Sight* SightConfig;
};
