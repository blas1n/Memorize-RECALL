// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ProjectRAiController.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FAiData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PatrolRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LoseSightRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FOV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightAge;
};

UCLASS()
class PROJECTR_API AProjectRAiController final : public AAIController
{
	GENERATED_BODY()
	
public:
	AProjectRAiController();

	UFUNCTION(BlueprintCallable)
	void RunAi(const FAiData& Data);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SetBlackboard(UBlackboardComponent* InBlackboard, const FAiData& Data);

private:
	void BeginPlay() override;

	UFUNCTION()
	void Death(AController* Attaker);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class APlayerCharacter* Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sense, meta = (AllowPrivateAccess = true))
	class UAISenseConfig_Sight* SightConfig;

	bool bIsInit;
};
