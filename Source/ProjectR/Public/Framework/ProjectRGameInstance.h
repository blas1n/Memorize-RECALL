// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ProjectRGameInstance.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTR_API UProjectRGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	class UDataTable* GetDataTable(FName Name) const;

private:
	UPROPERTY(EditDefaultsOnly, Category = Data, meta = (AllowPrivateAccess = true))
	TMap<FName, UDataTable*> DataTables;
};
