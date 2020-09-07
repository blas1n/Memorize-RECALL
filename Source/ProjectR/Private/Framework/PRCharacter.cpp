// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PRCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Component/PRMovementComponent.h"
#include "Component/StatComponent.h"
#include "Component/WeaponComponent.h"
#include "Data/CharacterData.h"
#include "Framework/PRAnimInstance.h"
#include "Library/PRStatics.h"

APRCharacter::APRCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UPRMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
	
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 1080.0f, 0.0f);
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat"));

	RightWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RightWeapon"));
	RightWeapon->SetupAttachment(GetMesh());
	RightWeapon->SetCollisionProfileName(TEXT("Weapon"));
	RightWeapon->SetAnimationMode(EAnimationMode::AnimationSingleNode);

	LeftWeapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LeftWeapon"));
	LeftWeapon->SetupAttachment(GetMesh());
	LeftWeapon->SetCollisionProfileName(TEXT("Weapon"));
	LeftWeapon->SetAnimationMode(EAnimationMode::AnimationSingleNode);

	WeaponComponent->SetRightWeapon(RightWeapon);
	WeaponComponent->SetLeftWeapon(LeftWeapon);

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_CharacterData.DT_CharacterData'"));
	CharacterDataTable = DataTable.Object;
}

void APRCharacter::Lock(AActor* NewLockTarget)
{
	ServerLock(NewLockTarget);
}

void APRCharacter::Unlock()
{
	ServerUnlock();
}

void APRCharacter::SetMoveState(EMoveState NewMoveState)
{
	ServerSetMoveState(NewMoveState);
}

#if WITH_EDITOR

void APRCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	static const FName& CharKeyName = GET_MEMBER_NAME_CHECKED(APRCharacter, CharacterKey);
	if (PropertyChangedEvent.Property && PropertyChangedEvent.GetPropertyName() == CharKeyName)
		Initialize();
}

#endif

void APRCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!LockTarget) return;

	if (!IsMoveInputIgnored())
	{
		const FVector MyLoc = GetActorLocation();
		const FVector TargetLoc = LockTarget->GetActorLocation();

		FRotator ActorRot = UKismetMathLibrary::FindLookAtRotation(MyLoc, TargetLoc);
		ActorRot = FMath::Lerp(GetActorRotation(), ActorRot, DeltaSeconds * 10.0f);
		SetActorRotation(ActorRot);
	}

	if (AController* MyController = GetController())
	{
		FVector MyEyeLoc; FRotator MyEyeRot;
		GetActorEyesViewPoint(MyEyeLoc, MyEyeRot);

		FVector TargetEyeLoc; FRotator TargetEyeRot;
		LockTarget->GetActorEyesViewPoint(TargetEyeLoc, TargetEyeRot);

		FRotator CtrlRot = UKismetMathLibrary::FindLookAtRotation(MyEyeLoc, TargetEyeLoc);
		CtrlRot = FMath::Lerp(MyController->GetControlRotation(), CtrlRot, DeltaSeconds * 8.0f);
		MyController->SetControlRotation(CtrlRot);
	}
}

void APRCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Initialize();
}

float APRCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{	
	Damage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (Damage <= 0.0f) return Damage;
	
	StatComponent->Heal(-Damage);
	if (StatComponent->GetHealth() <= 0.0f) Death();

	if (auto* InstigatorPawn = EventInstigator->GetPawn<APRCharacter>())
	{
		InstigatorPawn->OnAttack.Broadcast(Damage, this, DamageEvent.DamageTypeClass);
		OnDamage.Broadcast(Damage, InstigatorPawn, DamageEvent.DamageTypeClass);
	}

	return Damage;
}

bool APRCharacter::ShouldTakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser) const
{
	return
		Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser)
		&& EventInstigator && DamageCauser
		&& GetTeamAttitudeTowards(*EventInstigator) != ETeamAttitude::Friendly;
}

void APRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APRCharacter, MoveState);
	DOREPLIFETIME(APRCharacter, LockTarget);
	DOREPLIFETIME(APRCharacter, bIsLocked);
}

void APRCharacter::Landed(const FHitResult& Hit)
{
	OnLand.Broadcast(Hit);
}

void APRCharacter::Initialize()
{
	if (!CharacterDataTable)
	{
		CharacterKey = 0u;
		return;
	}

	const auto* Data = CharacterDataTable->FindRow<FCharacterData>(FName{ *FString::FromInt(CharacterKey) }, TEXT(""), false);
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found character data %d!"), CharacterKey);
		CharacterKey = 0u;
		return;
	}

	UPRStatics::AsyncLoad(Data->Mesh, [this, Data]
	{
		static const auto Rules = FAttachmentTransformRules::KeepRelativeTransform;

		GetCapsuleComponent()->SetCapsuleHalfHeight(Data->CapsuleHalfHeight);
		GetCapsuleComponent()->SetCapsuleRadius(Data->CapsuleRadius);

		GetMesh()->SetSkeletalMesh(Data->Mesh.Get());
		GetMesh()->SetAnimClass(Data->AnimClass);

		const FVector& Location = GetMesh()->GetRelativeLocation();
		const FRotator& Rotation = GetMesh()->GetRelativeRotation();

		GetMesh()->SetRelativeLocationAndRotation(
			FVector{ Location.X, Location.Y, Data->MeshZ },
			FRotator{ Rotation.Pitch, Data->MeshYaw, Rotation.Roll }
		);

		RightWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("hand_r"));
		LeftWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("hand_l"));
	});
}

void APRCharacter::Death()
{
	check(HasAuthority());
	WeaponComponent->StopSkill();
	MulticastDeath();
}

void APRCharacter::ServerSetMoveState_Implementation(EMoveState NewMoveState)
{
	MoveState = NewMoveState;
	SetMovement();
}

void APRCharacter::ServerLock_Implementation(AActor* NewLockTarget)
{
	LockTarget = NewLockTarget;

	const bool bWasLocked = bIsLocked;
	bIsLocked = true;

	if (!bWasLocked) SetMovement();
}

void APRCharacter::ServerUnlock_Implementation()
{
	LockTarget = nullptr;

	const bool bWasLocked = bIsLocked;
	bIsLocked = false;
	
	if (bWasLocked) SetMovement();
}

void APRCharacter::MulticastDeath_Implementation()
{
	bIsDeath = true;
	SetCanBeDamaged(false);
	GetController()->UnPossess();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);

	OnDeath.Broadcast();
}

void APRCharacter::OnRep_MoveState()
{
	SetMovement();
}

void APRCharacter::OnRep_IsLocked()
{
	SetMovement();
}

void APRCharacter::SetMovement()
{
	float Speed = StatComponent->GetWalkSpeed();
	bool bOrientRotationToMovement = true;
	bool bUseControllerDesiredRotation = false;

	if (MoveState == EMoveState::Run)
	{
		Speed = StatComponent->GetRunSpeed();
	}
	else if (bIsLocked)
	{
		Speed = StatComponent->GetLockSpeed();
		bUseControllerDesiredRotation = bOrientRotationToMovement = false;
	}

	auto* Movement = GetCharacterMovement();
	Movement->MaxWalkSpeed = Speed;
	Movement->bOrientRotationToMovement = bOrientRotationToMovement;
	Movement->bUseControllerDesiredRotation = bUseControllerDesiredRotation;
}

void APRCharacter::GetLookLocationAndRotation_Implementation(FVector& Location, FRotator& Rotation) const
{
	Super::GetActorEyesViewPoint(Location, Rotation);
}
