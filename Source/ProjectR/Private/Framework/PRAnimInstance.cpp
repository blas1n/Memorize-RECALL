// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PRAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"

UPRAnimInstance::UPRAnimInstance()
	: Super()
{
	BlendDuration = 0.5f;
	bIsLocking = false;
	bIsInAir = false;
}

void UPRAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const auto* User = Cast<APRCharacter>(GetOwningActor());
	if (!User) return;
	
	auto CurAnimData = User->GetWeaponComponent()->GetAnimData();
	if (AnimData.NotLock != CurAnimData.NotLock || CurAnimData.Lock != CurAnimData.Lock || CurAnimData.Air != CurAnimData.Air)
	{
		AnimData = MoveTemp(CurAnimData);
		SavePoseSnapshot(TEXT("Pose"));
		BlendRatio = 1.0f;
	}

	if (BlendRatio > 0.0f)
		BlendRatio = FMath::Max(BlendRatio - DeltaSeconds * (1.0f / BlendDuration), 0.0f);

	const auto* Movement = User->GetCharacterMovement();
	Velocity = FVector2D{ User->GetActorRotation().UnrotateVector(Movement->Velocity) };

	bIsLocking = User->IsLocked();
	bIsInAir = Movement->IsFalling();
}
