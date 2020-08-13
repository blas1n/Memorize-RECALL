// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify/AnimNotify_Execute.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"

void UAnimNotify_Execute::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (auto* Owner = Cast<APRCharacter>(MeshComp->GetOwner()))
		return Owner->GetWeaponComponent()->Execute();
}
