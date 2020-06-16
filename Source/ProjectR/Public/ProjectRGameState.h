// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ProjectRGameState.generated.h"

UCLASS()
class PROJECTR_API AProjectRGameState final : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	class UBuff* GetBuffByClass(TSubclassOf<UBuff> BuffClass);

	void InitBuffStorages(TMap<TSubclassOf<UBuff>, class UBuffStorage*>& BuffStorages);

private:
	void BeginPlay() override;

private:
	TMap<TSubclassOf<UBuff>, UBuff*> Buffs;
};
