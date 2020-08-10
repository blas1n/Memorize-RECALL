// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify/AnimNotifyState_Combo.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/WeaponComponent.h"
#include "Interface/ComponentOwner.h"

void UAnimNotifyState_Combo::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner && Owner->GetClass()->ImplementsInterface(UComponentOwner::StaticClass()))
		return IComponentOwner::Execute_GetWeaponComponent(Owner)->CheckCombo();
}

void UAnimNotifyState_Combo::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner && Owner->GetClass()->ImplementsInterface(UComponentOwner::StaticClass()))
		return IComponentOwner::Execute_GetWeaponComponent(Owner)->ExecuteCombo();
}
