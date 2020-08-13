// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify/AnimNotify_PlayParticle.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Component/WeaponComponent.h"
#include "Interface/ComponentOwner.h"

#if WITH_EDITOR

void UAnimNotify_PlayParticle::PreEditChange(UProperty* PropertyAboutToChange)
{
	if (!PropertyAboutToChange) return;

	if (PropertyAboutToChange->GetName() == GET_MEMBER_NAME_STRING_CHECKED(UAnimNotify_PlayParticle, Template) && Template != nullptr)
		PreviousTemplates.Add(Template);

	if (PropertyAboutToChange->GetName() == GET_MEMBER_NAME_STRING_CHECKED(UAnimNotify_PlayParticle, AttachParent))
		PreviousAttachParents.Add(AttachParent);

	if (PropertyAboutToChange->GetName() == GET_MEMBER_NAME_STRING_CHECKED(UAnimNotify_PlayParticle, SocketName) && SocketName != FName{ TEXT("None") })
		PreviousSocketNames.Add(SocketName);
}

#endif

void UAnimNotify_PlayParticle::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!Template || Template->IsLooping()) return;

	USceneComponent* Parent = nullptr;
	const bool bIsRightParent = GetParentComponent(MeshComp, Parent);

	FName Socket = SocketName;
	if (!bIsRightParent)
		Socket = AttachParent == EAttachParent::RightWeapon ? TEXT("weapon_r") : TEXT("weapon_l");;

	if (Parent)
	{
		if (!Parent->DoesSocketExist(Socket)) return;

		UNiagaraFunctionLibrary::SpawnSystemAttached(Template, Parent,
			Socket, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, true);
	}
	else
	{
		const FTransform Transform = Parent ? Parent->GetSocketTransform(Socket) : MeshComp->GetSocketTransform(Socket);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp->GetWorld(), Template,
			Transform.TransformPosition(LocationOffset), Transform.TransformRotation(FQuat{ RotationOffset }).Rotator());
	}
}

FString UAnimNotify_PlayParticle::GetNotifyName_Implementation() const
{
	return Template ? Template->GetName() : Super::GetNotifyName_Implementation();
}

bool UAnimNotify_PlayParticle::GetParentComponent(USkeletalMeshComponent* MeshComp, USceneComponent*& Parent) const
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

	auto* Owner = Cast<APRCharacter>(MeshComp->GetOwner());
	if (!Owner)
	{
		Parent = MeshComp;
		return false;
	}

	auto* WeaponComp = Owner->GetWeaponComponent();
	if (AttachParent == EAttachParent::RightWeapon)
		Parent = WeaponComp->GetRightWeapon();
	else
		Parent = WeaponComp->GetLeftWeapon();

	return Parent != nullptr;
}
