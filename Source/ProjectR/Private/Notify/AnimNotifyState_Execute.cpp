// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify/AnimNotifyState_Execute.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/WeaponComponent.h"
#include "Interface/ComponentOwner.h"

void UAnimNotifyState_Execute::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner && Owner->GetClass()->ImplementsInterface(UComponentOwner::StaticClass()))
		return IComponentOwner::Execute_GetWeaponComponent(Owner)->BeginExecute();
}

void UAnimNotifyState_Execute::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner && Owner->GetClass()->ImplementsInterface(UComponentOwner::StaticClass()))
		return IComponentOwner::Execute_GetWeaponComponent(Owner)->EndExecute();
}
