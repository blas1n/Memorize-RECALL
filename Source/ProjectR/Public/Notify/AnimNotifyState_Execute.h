// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_Execute.generated.h"

UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Execute"))
class PROJECTR_API UAnimNotifyState_Execute : public UAnimNotifyState
{
	GENERATED_BODY()
	
private:
	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
