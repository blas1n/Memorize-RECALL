// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify/AnimNotifyState_PlayParticle.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"
#include "Interface/ComponentOwner.h"

#if WITH_EDITOR

void UAnimNotifyState_PlayParticle::PreEditChange(UProperty* PropertyAboutToChange)
{
	if (!PropertyAboutToChange) return;

	if (PropertyAboutToChange->GetName() == GET_MEMBER_NAME_STRING_CHECKED(UAnimNotifyState_PlayParticle, Template) && Template != nullptr)
		PreviousTemplates.Add(Template);

	if (PropertyAboutToChange->GetName() == GET_MEMBER_NAME_STRING_CHECKED(UAnimNotifyState_PlayParticle, AttachParent))
		PreviousAttachParents.Add(AttachParent);

	if (PropertyAboutToChange->GetName() == GET_MEMBER_NAME_STRING_CHECKED(UAnimNotifyState_PlayParticle, SocketName) && SocketName != FName{ TEXT("None") })
		PreviousSocketNames.Add(SocketName);
}

#endif

void UAnimNotifyState_PlayParticle::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!Template) return;

	USceneComponent* Parent = nullptr;
	const bool bHaveParent = GetParentComponent(MeshComp, Parent);

	FName Socket = SocketName;
	if (!bHaveParent)
		Socket = AttachParent == EAttachParent::RightWeapon ? TEXT("weapon_r") : TEXT("weapon_l");

	if (!Parent || !Parent->DoesSocketExist(Socket)) return;
	
	UNiagaraFunctionLibrary::SpawnSystemAttached(Template, Parent,
		Socket, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, true);
}

void UAnimNotifyState_PlayParticle::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	USceneComponent* Parent = nullptr;
	const bool bHaveParent = GetParentComponent(MeshComp, Parent);
	
	FName Socket = SocketName;
	if (!bHaveParent)
		Socket = AttachParent == EAttachParent::RightWeapon ? TEXT("weapon_r") : TEXT("weapon_l");
	
	if (!Parent || !Parent->DoesSocketExist(Socket)) return;

	TArray<USceneComponent*> Children;
	Parent->GetChildrenComponents(false, Children);

	for (USceneComponent* Component : Children)
	{
		auto* FXSystemComponent = Cast<UFXSystemComponent>(Component);
		if (!FXSystemComponent) continue;
		
		bool bSocketMatch = FXSystemComponent->GetAttachSocketName() == Socket;
		bool bTemplateMatch = FXSystemComponent->GetFXSystemAsset() == Template;

#if WITH_EDITORONLY_DATA
		bSocketMatch |= PreviousSocketNames.Contains(FXSystemComponent->GetAttachSocketName());
		bTemplateMatch |= PreviousTemplates.Contains(FXSystemComponent->GetFXSystemAsset());
#endif

		if (!bSocketMatch || !bTemplateMatch || !FXSystemComponent->IsActive())
			continue;

		FXSystemComponent->Deactivate();

#if WITH_EDITORONLY_DATA
		PreviousTemplates.Empty();
		PreviousSocketNames.Empty();
#endif
	}
}

FString UAnimNotifyState_PlayParticle::GetNotifyName_Implementation() const
{
	return Template ? Template->GetName() : Super::GetNotifyName_Implementation();
}

bool UAnimNotifyState_PlayParticle::GetParentComponent(USkeletalMeshComponent* MeshComp, USceneComponent*& Parent) const
{
	if (AttachParent == EAttachParent::None)
	{
		Parent = nullptr;
		return true;
	}
	if (AttachParent == EAttachParent::Mesh)
	{
		Parent = MeshComp;
		return true;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner || Owner->GetClass()->ImplementsInterface(UComponentOwner::StaticClass()))
	{
		Parent = MeshComp;
		return false;
	}

	auto* WeaponComp = IComponentOwner::Execute_GetWeaponComponent(Owner);
	if (AttachParent == EAttachParent::RightWeapon)
		Parent = WeaponComp->GetRightWeapon();
	else
		Parent = WeaponComp->GetLeftWeapon();

	return Parent != nullptr;
}
