// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIManagerComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTR_API UAIManagerComponent final : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIManagerComponent();

private:
#if WITH_EDITOR
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void BeginPlay() override;
	void Initialize();

private:
	UPROPERTY(EditAnywhere, Category = AI, meta = (AllowPrivateAccess = true))
	TAssetPtr<class UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, Category = AI, meta = (AllowPrivateAccess = true))
	int32 LogicKey;

	UPROPERTY()
	class UDataTable* LogicDataTable;
};
