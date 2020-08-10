// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_Execute.generated.h"

UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Execute"))
class PROJECTR_API UAnimNotify_Execute : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
