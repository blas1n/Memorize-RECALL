// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ProjectRAIController.generated.h"

UCLASS()
class PROJECTR_API AProjectRAIController final : public AAIController
{
	GENERATED_BODY()

public:
	AProjectRAIController();

private:
	void OnPossess(APawn* InPawn) override;
	void OnUnPossess() override;

	void Tick(float DeltaSeconds) override;

	bool GetMoveTargetLocation(FVector& Out) const;

private:
	UPROPERTY()
	class AProjectRCharacter* User;
};
