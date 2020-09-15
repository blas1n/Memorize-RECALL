// Fill out your copyright notice in the Description page of Project Settings.

#include "Misc/Spline.h"

bool USpline::Compute(int32 Idx, float Alpha, FVector& Point)
{
	if (!IsValidIndex(Idx)) return false;

	const FVector& P0 = Points[Idx - 1];
	const FVector& P1 = Points[Idx];
	const FVector& P2 = Points[Idx + 1];
	const FVector& P3 = Points[Idx + 2];

	Point = 0.5f * ((2.0f * P1) + (-P0 + P2) * Alpha +
		(2.0f * P0 - 5.0f * P1 + 4.0f * P2 - P3) * Alpha * Alpha +
		(-P0 + 3.0f * P1 - 3.0f * P2 + P3) * Alpha * Alpha * Alpha);

	return true;
}

bool USpline::IsValidIndex(int32 Idx)
{
	return Idx != 0 && Idx + 2 < Points.Num();
}
