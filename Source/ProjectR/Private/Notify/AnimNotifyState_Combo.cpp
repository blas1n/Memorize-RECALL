// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify/AnimNotifyState_Combo.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"

void UAnimNotifyState_Combo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (auto* Owner = Cast<APRCharacter>(MeshComp->GetOwner()))
		return Owner->GetWeaponComponent()->CheckCombo();
}

void UAnimNotifyState_Combo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (auto* Owner = Cast<APRCharacter>(MeshComp->GetOwner()))
		return Owner->GetWeaponComponent()->ExecuteCombo();
}
