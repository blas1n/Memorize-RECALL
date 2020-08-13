// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify/AnimNotifyState_Execute.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"

void UAnimNotifyState_Execute::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (auto* Owner = Cast<APRCharacter>(MeshComp->GetOwner()))
		return Owner->GetWeaponComponent()->BeginExecute();
}

void UAnimNotifyState_Execute::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (auto* Owner = Cast<APRCharacter>(MeshComp->GetOwner()))
		return Owner->GetWeaponComponent()->EndExecute();
}
