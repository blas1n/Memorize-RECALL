// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Data/AttachParent.h"
#include "AnimNotifyState_PlayParticle.generated.h"

UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Play Particle"))
class PROJECTR_API UAnimNotifyState_PlayParticle final : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
#if WITH_EDITOR
	void PreEditChange(UProperty* PropertyAboutToChange) override;
#endif

	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	FString GetNotifyName_Implementation() const override;

private:
	bool GetParentComponent(USkeletalMeshComponent* MeshComp, USceneComponent*& Parent) const;

private:
	UPROPERTY(EditAnywhere, Category = ParticleSystem, meta = (AllowPrivateAccess = true))
	class UNiagaraSystem* Template;

	UPROPERTY(EditAnywhere, Category = ParticleSystem, meta = (AllowPrivateAccess = true))
	EAttachParent AttachParent;

	UPROPERTY(EditAnywhere, Category = ParticleSystem, meta = (AllowPrivateAccess = true))
	FName SocketName;

	UPROPERTY(EditAnywhere, Category = ParticleSystem, meta = (AllowPrivateAccess = true))
	FVector LocationOffset;

	UPROPERTY(EditAnywhere, Category = ParticleSystem, meta = (AllowPrivateAccess = true))
	FRotator RotationOffset;

#if WITH_EDITORONLY_DATA

	UPROPERTY(transient)
	TArray<class UFXSystemAsset*> PreviousTemplates;

	UPROPERTY(transient)
	TArray<EAttachParent> PreviousAttachParents;

	UPROPERTY(transient)
	TArray<FName> PreviousSocketNames;

#endif
};
