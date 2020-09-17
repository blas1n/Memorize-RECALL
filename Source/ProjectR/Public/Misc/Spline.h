// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Spline.generated.h"

UCLASS(BlueprintType)
class PROJECTR_API USpline : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool Compute(int32 Idx, float Alpha, FVector& Point);

	UFUNCTION(BlueprintCallable)
	bool IsValidIndex(int32 Idx);

	FORCEINLINE TArray<FVector>& GetPoints() noexcept { return Points; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TArray<FVector> Points;
};
