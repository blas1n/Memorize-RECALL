// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify/AnimNotify_Execute.h"
#include "Components/SkeletalMeshComponent.h"
#include "Component/WeaponComponent.h"
#include "Interface/ComponentOwner.h"

void UAnimNotify_Execute::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AActor* Owner = MeshComp->GetOwner();
	if (Owner && Owner->GetClass()->ImplementsInterface(UComponentOwner::StaticClass()))
		return IComponentOwner::Execute_GetWeaponComponent(Owner)->Execute();
}
