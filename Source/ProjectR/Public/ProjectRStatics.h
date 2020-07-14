// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/AssetManager.h"
#include "ProjectRStatics.generated.h"

UCLASS()
class PROJECTR_API UProjectRStatics final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	template <class T, class Func>
	static void AsyncLoad(const TAssetPtr<T>& SoftPtr, Func&& Fn)
	{
		if (SoftPtr.IsNull())
			return;

		if (SoftPtr.IsPending())
		{
			auto Callback = FStreamableDelegate::CreateLambda(Forward<Func>(Fn));
			UAssetManager::GetStreamableManager().RequestAsyncLoad(SoftPtr.ToSoftObjectPath(), MoveTemp(Callback));
		}
		else Fn();
	}
};
