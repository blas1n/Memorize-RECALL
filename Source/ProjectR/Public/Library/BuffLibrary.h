// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BuffLibrary.generated.h"

UCLASS()
class PROJECTR_API UBuffLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static class UBuff* GetBuff(class AActor* Target, TSubclassOf<UBuff> BuffClass);
	static const UBuff* GetBuff(const AActor* Target, TSubclassOf<UBuff> BuffClass);

	template <class T>
	FORCEINLINE static T* GetBuff(AActor* Target)
	{
		return Cast<T>(GetBuff(Target, T::StaticClass()));
	}

	template <class T>
	FORCEINLINE static const T* GetBuff(const AActor* Target)
	{
		return Cast<T>(GetBuff(Target, T::StaticClass()));
	}
	
	UFUNCTION(BlueprintCallable)
	static void ApplyBuff(AActor* Target, TSubclassOf<UBuff> BuffClass);

	template <class T>
	FORCEINLINE static void ApplyBuff(AActor* Target)
	{
		ApplyBuff(Target, T::StaticClass());
	}

	UFUNCTION(BlueprintCallable)
	static void ReleaseBuff(AActor* Target, TSubclassOf<UBuff> BuffClass);

	template <class T>
	FORCEINLINE static void ReleaseBuff(AActor* Target)
	{
		ReleaseBuff(Target, T::StaticClass());
	}

	UFUNCTION(BlueprintCallable)
	static void ApplyBuffWithDuration(AActor* Target, TSubclassOf<UBuff> BuffClass, float Duration);

	template <class T>
	FORCEINLINE static void ApplyBuffWithDuration(AActor* Target, float Duration)
	{
		ApplyBuffWithDuration(Target, T::StaticClass(), Duration);
	}

	UFUNCTION(BlueprintCallable)
	static bool IsActivate(const AActor* Target, TSubclassOf<UBuff> BuffClass);

	template <class T>
	FORCEINLINE static bool IsActivate(const AActor* Target)
	{
		return IsActivate(Target, T::StaticClass());
	}
};
