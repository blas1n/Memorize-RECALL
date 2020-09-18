// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PRPlayerController.h"
#include "Engine/World.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Component/PRMovementComponent.h"
#include "Component/TargetComponent.h"
#include "Component/WeaponComponent.h"
#include "Framework/PRCharacter.h"
#include "Interface/Interactable.h"

DECLARE_DELEGATE_OneParam(FIndexer, uint8);

APRPlayerController::APRPlayerController()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;
	bAllowTickBeforeBeginPlay = false;

	Targeter = CreateDefaultSubobject<UTargetComponent>(TEXT("Targeter"));
	Perception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
}

void APRPlayerController::RegisterInteractor(UObject* InInteractor)
{
	check(InInteractor->GetClass()->ImplementsInterface(UInteractable::StaticClass()));
	Interactor = InInteractor;
	OnSetInteractor(Interactor);
}

void APRPlayerController::UnregisterInteractor()
{
	Interactor = nullptr;
	OnSetInteractor(Interactor);
}

FGenericTeamId APRPlayerController::GetGenericTeamId() const
{
	if (auto* MyPawn = GetPawn<IGenericTeamAgentInterface>())
		return MyPawn->GetGenericTeamId();

	return FGenericTeamId::NoTeam;
}

void APRPlayerController::SetGenericTeamId(const FGenericTeamId& NewTeamId)
{
	if (auto* MyPawn = GetPawn<IGenericTeamAgentInterface>())
		return MyPawn->SetGenericTeamId(NewTeamId);
}

void APRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	Targeter->Initialize(Perception);
}

void APRPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	auto* MyPawn = GetPawn<APRCharacter>();
	if (!MyPawn || !MyPawn->GetLockedTarget()) return;

	const AActor* LockedTarget = MyPawn->GetLockedTarget();
	if (Cast<APRCharacter>(LockedTarget)->IsDeath())
	{
		Relock();
		return;
	}

	float Radius = 0.0f;
	for (auto Iter = Perception->GetSensesConfigIterator(); Iter; ++Iter)
	{
		if (auto* Config = Cast<UAISenseConfig_Sight>(*Iter))
		{
			Radius = Config->SightRadius;
			break;
		}
	}

	const FVector TargetLoc = LockedTarget->GetActorLocation();
	const float Dist = FVector::DistSquared(MyPawn->GetActorLocation(), TargetLoc);
	if (Dist > FMath::Square(Radius))
		Relock();
}

void APRPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis(TEXT("MoveForward"), this, &APRPlayerController::MoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &APRPlayerController::MoveRight);

	InputComponent->BindAxis(TEXT("Turn"), this, &APRPlayerController::InputYaw);
	InputComponent->BindAxis(TEXT("LookUp"), this, &APRPlayerController::InputPitch);

	InputComponent->BindAxis(TEXT("Swap"), this, &APRPlayerController::SwapWeapon);

	InputComponent->BindAction(TEXT("Dodge"), IE_Pressed, this, &APRPlayerController::PressDodge);
	InputComponent->BindAction(TEXT("Dodge"), IE_Released, this, &APRPlayerController::ReleaseDodge);

	InputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &APRPlayerController::Run);
	InputComponent->BindAction(TEXT("Run"), IE_Released, this, &APRPlayerController::Walk);

	InputComponent->BindAction(TEXT("WeekAttack"), IE_Pressed, this, &APRPlayerController::AttackWeak);
	InputComponent->BindAction(TEXT("StrongAttack"), IE_Pressed, this, &APRPlayerController::AttackStrong);
	
	InputComponent->BindAction<FIndexer>(TEXT("Weapon1"), IE_Pressed, this, &APRPlayerController::SwapWeapon, static_cast<uint8>(0));
	InputComponent->BindAction<FIndexer>(TEXT("Weapon2"), IE_Pressed, this, &APRPlayerController::SwapWeapon, static_cast<uint8>(1));
	InputComponent->BindAction<FIndexer>(TEXT("Weapon3"), IE_Pressed, this, &APRPlayerController::SwapWeapon, static_cast<uint8>(2));

	InputComponent->BindAction(TEXT("Lock"), IE_Pressed, this, &APRPlayerController::Lock);
	InputComponent->BindAction(TEXT("Lock"), IE_Released, this, &APRPlayerController::Unlock);

	InputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &APRPlayerController::Interact);
}

void APRPlayerController::MoveForward(float Value)
{
	if (Value != 0.0f)
		if (auto* MyPawn = GetPawn())
			MyPawn->AddMovementInput(GetDirectionVector(EAxis::X), Value);
}

void APRPlayerController::MoveRight(float Value)
{
	if (Value != 0.0f)
		if (auto* MyPawn = GetPawn())
			MyPawn->AddMovementInput(GetDirectionVector(EAxis::Y), Value);
}

void APRPlayerController::InputYaw(float Value)
{
	if (GetPawn()) AddYawInput(Value);
}

void APRPlayerController::InputPitch(float Value)
{
	if (GetPawn()) AddPitchInput(Value);
}

void APRPlayerController::PressDodge()
{
	auto* MyPawn = GetPawn<APRCharacter>();
	if (!MyPawn) return;

	if (MyPawn->IsLocked())
		MyPawn->GetWeaponComponent()->Dodge();
	else if (!IsMoveInputIgnored())
		MyPawn->Jump();
}

void APRPlayerController::ReleaseDodge()
{
	auto* MyPawn = GetPawn<ACharacter>();
	if (MyPawn && MyPawn->GetCharacterMovement()->IsFalling())
		MyPawn->StopJumping();
}

void APRPlayerController::Run()
{
	if (auto* MyPawn = GetPawn<APRCharacter>())
	{
		Cast<UPRMovementComponent>(MyPawn->
			GetCharacterMovement())->SetMoveState(EMoveState::Run);
	}
}

void APRPlayerController::Walk()
{
	if (auto* MyPawn = GetPawn<APRCharacter>())
	{
		Cast<UPRMovementComponent>(MyPawn->
			GetCharacterMovement())->SetMoveState(EMoveState::Walk);
	}
}

void APRPlayerController::AttackWeak()
{
	if (auto* MyPawn = GetPawn<APRCharacter>())
		return MyPawn->GetWeaponComponent()->Attack(false);
}

void APRPlayerController::AttackStrong()
{
	if (auto* MyPawn = GetPawn<APRCharacter>())
		return MyPawn->GetWeaponComponent()->Attack(true);
}

void APRPlayerController::SwapWeapon(uint8 Index)
{
	if (auto* MyPawn = GetPawn<APRCharacter>())
		return MyPawn->GetWeaponComponent()->SwapWeapon(Index);
}

void APRPlayerController::SwapWeapon(float Value)
{
	if (Value == 0.0f) return;

	APRCharacter* MyPawn = GetPawn<APRCharacter>();
	if (!MyPawn) return;

	auto* WeaponComp = MyPawn->GetWeaponComponent();
	const int32 WeaponNum = WeaponComp->GetWeaponNum();
	if (WeaponNum == 0) return;

	int32 Idx = static_cast<int32>(WeaponComp->GetWeaponIndex());
	Idx += FMath::RoundFromZero(Value);
	Idx = ((Idx % WeaponNum) + WeaponNum) % WeaponNum;
	
	SwapWeapon(static_cast<uint8>(Idx));
}

void APRPlayerController::Lock()
{
	auto* MyPawn = GetPawn<APRCharacter>();
	if (!MyPawn) return;
	
	AActor* Target = Targeter->GetTargetedActor();
	MyPawn->Lock(Target);

	if (!Target)
		SetControlRotation(FRotator{ 0.0f, MyPawn->GetActorRotation().Yaw, 0.0f });
}

void APRPlayerController::Unlock()
{
	if (APRCharacter* MyPawn = GetPawn<APRCharacter>())
		MyPawn->Unlock();
}

void APRPlayerController::Relock()
{
	auto* MyPawn = GetPawn<APRCharacter>();
	if (!MyPawn) return;

	if (AActor* Target = Targeter->GetTargetedActor())
		MyPawn->Lock(Target);
	else
		MyPawn->Unlock();
}

void APRPlayerController::Interact()
{
	if (!Interactor) return;
	
	IInteractable::Execute_Interact(Interactor, GetPawn<APRCharacter>());
	Interactor = nullptr;
}

FVector APRPlayerController::GetDirectionVector(EAxis::Type Axis) const
{
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	return FRotationMatrix(YawRotation).GetUnitAxis(Axis);
}
