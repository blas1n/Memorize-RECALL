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
	static class UBuff* GetBuff(class AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass);
	static const UBuff* GetBuff(const AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass);

	template <class T>
	FORCEINLINE static T* GetBuff(AProjectRCharacter* Character)
	{
		return Cast<T>(GetBuff(Character, T::StaticClass()));
	}

	template <class T>
	FORCEINLINE static const T* GetBuff(const AProjectRCharacter* Character)
	{
		return Cast<T>(GetBuff(Character, T::StaticClass()));
	}
	
	UFUNCTION(BlueprintCallable)
	static void ApplyBuff(AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass);

	template <class T>
	FORCEINLINE static void ApplyBuff(AProjectRCharacter* Character)
	{
		ApplyBuff(Character, T::StaticClass());
	}

	UFUNCTION(BlueprintCallable)
	static void ReleaseBuff(AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass);

	template <class T>
	FORCEINLINE static void ReleaseBuff(AProjectRCharacter* Character)
	{
		ReleaseBuff(Character, T::StaticClass());
	}

	UFUNCTION(BlueprintCallable)
	static void ApplyBuffWithDuration(AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass, float Duration);

	template <class T>
	FORCEINLINE static void ApplyBuffWithDuration(AProjectRCharacter* Character, float Duration)
	{
		ApplyBuffWithDuration(Character, T::StaticClass(), Duration);
	}

	UFUNCTION(BlueprintCallable)
	static void BlockBuff(AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass);

	template <class T>
	FORCEINLINE static void BlockBuff(AProjectRCharacter* Character)
	{
		BlockBuff(Character, T::StaticClass());
	}

	UFUNCTION(BlueprintCallable)
	static void UnblockBuff(AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass);

	template <class T>
	FORCEINLINE static void UnblockBuff(AProjectRCharacter* Character)
	{
		UnblockBuff(Character, T::StaticClass());
	}

	UFUNCTION(BlueprintCallable)
	static bool IsActivate(const AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass);

	template <class T>
	FORCEINLINE static bool IsActivate(const AProjectRCharacter* Character)
	{
		return IsActivate(Character, T::StaticClass());
	}

	UFUNCTION(BlueprintCallable)
	static bool IsBlocked(const AProjectRCharacter* Character, TSubclassOf<UBuff> BuffClass);

	template <class T>
	FORCEINLINE static bool IsBlocked(const AProjectRCharacter* Character)
	{
		return IsBlocked(Character, T::StaticClass());
	}
};
