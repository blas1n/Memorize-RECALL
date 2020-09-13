// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/PRMovementComponent.h"
#include "Net/UnrealNetwork.h"

UPRMovementComponent::UPRMovementComponent()
	: Super()
{
	SetIsReplicatedByDefault(true);

	GravityScale = 2.0f;
	JumpZVelocity = 420.f;
	AirControl = 0.05f;
	RotationRate = FRotator(0.0f, 360.0f, 0.0f);
	bUseControllerDesiredRotation = false;
	bOrientRotationToMovement = true;
	bAllowPhysicsRotationDuringAnimRootMotion = true;

	RunSpeed = 600.0f;
	WalkSpeed = 400.0f;
	LockSpeed = 300.0f;
}

void UPRMovementComponent::SetRunSpeed(float InRunSpeed)
{
	ServerSetRunSpeed(InRunSpeed);
}

void UPRMovementComponent::SetWalkSpeed(float InWalkSpeed)
{
	ServerSetWalkSpeed(InWalkSpeed);
}

void UPRMovementComponent::SetLockSpeed(float InLockSpeed)
{
	ServerSetLockSpeed(InLockSpeed);
}

void UPRMovementComponent::SetMoveState(EMoveState NewMoveState)
{
	if (MoveState != NewMoveState)
		ServerSetMoveState(NewMoveState);
}

void UPRMovementComponent::ApplyLock(bool bIsLock)
{
	bIsLocked = bIsLock;
	SetMovement();
}

void UPRMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	SetMovement();
}

void UPRMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPRMovementComponent, RunSpeed);
	DOREPLIFETIME(UPRMovementComponent, WalkSpeed);
	DOREPLIFETIME(UPRMovementComponent, LockSpeed);
	DOREPLIFETIME(UPRMovementComponent, MoveState);
}

void UPRMovementComponent::AddInputVector(FVector WorldVector, bool bForce)
{
	Super::AddInputVector(WorldVector, bForce);
	InputVector += WorldVector;
}

FVector UPRMovementComponent::ConsumeInputVector()
{
	if (GetOwner()->HasLocalNetOwner())
	{
		LastInputVector = InputVector;
		InputVector = FVector::ZeroVector;
	}

	if (GetOwnerRole() == ROLE_AutonomousProxy)
		ServerSetLastInputVector(LastInputVector);

	return Super::ConsumeInputVector();
}

void UPRMovementComponent::ServerSetRunSpeed_Implementation(float InRunSpeed)
{
	RunSpeed = InRunSpeed;
	OnRep_RunSpeed();
}

void UPRMovementComponent::ServerSetWalkSpeed_Implementation(float InWalkSpeed)
{
	WalkSpeed = InWalkSpeed;
	OnRep_WalkSpeed();
}

void UPRMovementComponent::ServerSetLockSpeed_Implementation(float InLockSpeed)
{
	LockSpeed = InLockSpeed;
	OnRep_LockSpeed();
}

void UPRMovementComponent::ServerSetMoveState_Implementation(EMoveState NewMoveState)
{
	MoveState = NewMoveState;
	OnRep_MoveState();
}

void UPRMovementComponent::OnRep_RunSpeed()
{
	if (MoveState == EMoveState::Run)
		MaxWalkSpeed = RunSpeed;
}

void UPRMovementComponent::OnRep_WalkSpeed()
{
	if (MoveState == EMoveState::Walk)
		MaxWalkSpeed = WalkSpeed;
}

void UPRMovementComponent::OnRep_LockSpeed()
{
	if (bIsLocked || MoveState == EMoveState::Walk)
		MaxWalkSpeed = LockSpeed;
}

void UPRMovementComponent::OnRep_MoveState()
{
	SetMovement();
}

void UPRMovementComponent::SetMovement()
{
	MaxWalkSpeed = WalkSpeed;
	bOrientRotationToMovement = true;
	bUseControllerDesiredRotation = false;

	if (MoveState == EMoveState::Run)
	{
		MaxWalkSpeed = RunSpeed;
	}
	else if (bIsLocked)
	{
		MaxWalkSpeed = LockSpeed;
		bUseControllerDesiredRotation = bOrientRotationToMovement = false;
	}
}
