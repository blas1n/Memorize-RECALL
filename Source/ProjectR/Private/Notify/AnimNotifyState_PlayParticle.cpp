// Fill out your copyright notice in the Description page of Project Settings.

#include "Notify/AnimNotifyState_PlayParticle.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/ProjectRCharacter.h"
#include "WeaponComponent.h"

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
	const bool bIsRightParent = GetParentComponent(MeshComp, Parent);

	FName Socket = SocketName;
	if (!bIsRightParent)
		Socket = AttachParent == EAttachParent::RightWeapon ? TEXT("weapon_r") : TEXT("weapon_l");;

	if (!Parent || !Parent->DoesSocketExist(Socket)) return;
	
	if (Parent)
	{
		FXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(Template, Parent,
			Socket, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, true);
	}
	else
	{
		const FTransform Transform = Parent ? Parent->GetSocketTransform(Socket) : MeshComp->GetSocketTransform(Socket);
		FXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(MeshComp->GetWorld(), Template,
			Transform.TransformPosition(LocationOffset), Transform.TransformRotation(FQuat{ RotationOffset }).Rotator());
	}
}

void UAnimNotifyState_PlayParticle::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (FXComponent && FXComponent->IsActive())
		FXComponent->Deactivate();
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

	auto* Owner = Cast<AProjectRCharacter>(MeshComp->GetOwner());
	if (!Owner)
	{
		Parent = MeshComp;
		return false;
	}

	if (AttachParent == EAttachParent::RightWeapon)
		Parent = Owner->GetWeaponComponent()->GetRightWeapon();
	else
		Parent = Owner->GetWeaponComponent()->GetLeftWeapon();

	return Parent != nullptr;
}
