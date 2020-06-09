// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectRGameMode.generated.h"

UCLASS(minimalapi)
class AProjectRGameMode final : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	class ABuff* GetBuffByClass(TSubclassOf<ABuff> BuffClass);

private:
	void BeginPlay() override;

private:
	TMap<TSubclassOf<ABuff>, ABuff*> Buffs;
};