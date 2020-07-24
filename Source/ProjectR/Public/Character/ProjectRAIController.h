// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ProjectRAIController.generated.h"

UCLASS(Blueprintable)
class PROJECTR_API AProjectRAIController final : public AAIController
{
	GENERATED_BODY()

public:
	AProjectRAIController();

	void InitLogic(const TAssetPtr<UBehaviorTree>& BehaviorTree, const struct FLogicData& LogicData);

protected:
	virtual void InitializeLogic(class UDataAsset* LogicData) {}

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = InitLogic))
	void ReceiveInitializeLogic(class UDataAsset* LogicData);

private:
	void OnPossess(APawn* InPawn) override;
	void OnUnPossess() override;

	void InitBlackboard(const FLogicData& LogicData);

	UFUNCTION()
	void OnDeath(AController* LastInstigator);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	class UNavigationInvokerComponent* NavigationInvoker;
};
