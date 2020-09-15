// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PRCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "Perception/AISense_Damage.h"
#include "Component/PRMovementComponent.h"
#include "Component/WeaponComponent.h"
#include "Component/WeaponMeshComponent.h"
#include "Data/CharacterData.h"
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
	
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_CharacterData.DT_CharacterData'"));
	CharacterDataTable = DataTable.Object;

	RightWeapon = CreateDefaultSubobject<UWeaponMeshComponent>(TEXT("RightWeapon"));
	LeftWeapon = CreateDefaultSubobject<UWeaponMeshComponent>(TEXT("LeftWeapon"));
	RightWeapon->SetupAttachment(GetMesh());
	LeftWeapon->SetupAttachment(GetMesh());

	WeaponComp = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	WeaponComp->SetWeaponComponent(RightWeapon, LeftWeapon);
}

void APRCharacter::Heal(float Value)
{
	check(HasAuthority());
	Health = FMath::Clamp(Health + Value, 0.0f, MaxHealth);
}

void APRCharacter::SetMaxHealth(float NewMaxHealth, bool bWithCurrent)
{
	check(HasAuthority());
	
	const float Delta = MaxHealth - Health;
	MaxHealth = NewMaxHealth;

	if (bWithCurrent)
		Health = MaxHealth - Delta;
}

void APRCharacter::Lock(AActor* NewLockTarget)
{
	ServerLock(NewLockTarget);
}

void APRCharacter::Unlock()
{
	ServerUnlock();
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

	if (!LockedTarget) return;

	if (!IsMoveInputIgnored())
	{
		const FVector MyLoc = GetActorLocation();
		const FVector TargetLoc = LockedTarget->GetActorLocation();

		FRotator ActorRot = UKismetMathLibrary::FindLookAtRotation(MyLoc, TargetLoc);
		ActorRot = FMath::Lerp(GetActorRotation(), ActorRot, DeltaSeconds * 10.0f);
		SetActorRotation(FRotator{ 0.0f, ActorRot.Yaw, 0.0f });
	}

	if (AController* MyController = GetController())
	{
		FVector MyEyeLoc; FRotator MyEyeRot;
		GetActorEyesViewPoint(MyEyeLoc, MyEyeRot);

		FVector TargetEyeLoc; FRotator TargetEyeRot;
		LockedTarget->GetActorEyesViewPoint(TargetEyeLoc, TargetEyeRot);

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

void APRCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		WeaponComp->SetComponents(GetAttackComponents());
		Health = MaxHealth;
	}
}

float APRCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{	
	Damage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (Damage <= 0.0f) return 0.0f;

	Health = FMath::Max(Health - Damage, 0.0f);
	if (Health <= 0.0f) Death();

	if (auto* InstigatorPawn = EventInstigator->GetPawn<APRCharacter>())
	{
		MulticastOnHit(Damage, InstigatorPawn);
		UAISense_Damage::ReportDamageEvent(GetWorld(), this, InstigatorPawn,
			Damage, InstigatorPawn->GetActorLocation(), GetActorLocation());
	}

	return Damage;
}

bool APRCharacter::ShouldTakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser) const
{
	return
		Super::ShouldTakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser)
		&& EventInstigator && DamageCauser && Damage > 0.0f
		&& GetTeamAttitudeTowards(*DamageCauser) != ETeamAttitude::Friendly;
}

void APRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APRCharacter, LockedTarget);
	DOREPLIFETIME(APRCharacter, Health);
	DOREPLIFETIME(APRCharacter, MaxHealth);
	DOREPLIFETIME(APRCharacter, bIsLocked);
}

void APRCharacter::Initialize()
{
	if (CharacterDataTable)
	{
		if (const auto* Data = CharacterDataTable->FindRow<FCharacterData>
			(FName{ *FString::FromInt(CharacterKey) }, TEXT(""), false))
		{
			UPRStatics::AsyncLoad(Data->Mesh, [this, Data] { ApplyCharacterData(*Data); });
			return;
		}
		
		UE_LOG(LogDataTable, Error, TEXT("Cannot found character data %d!"), CharacterKey);
	}

	ApplyCharacterData(FCharacterData{});
	CharacterKey = 0u;
}

void APRCharacter::Death()
{
	check(HasAuthority());
	WeaponComp->StopSkill();
	MulticastDeath();
}

void APRCharacter::ServerLock_Implementation(AActor* NewLockTarget)
{
	LockedTarget = NewLockTarget;

	const bool bWasLocked = bIsLocked;
	bIsLocked = true;

	if (!bWasLocked)
		Cast<UPRMovementComponent>(GetCharacterMovement())->ApplyLock(true);
}

void APRCharacter::ServerUnlock_Implementation()
{
	LockedTarget = nullptr;

	const bool bWasLocked = bIsLocked;
	bIsLocked = false;
	
	if (bWasLocked)
		Cast<UPRMovementComponent>(GetCharacterMovement())->ApplyLock(false);
}

void APRCharacter::MulticastOnHit_Implementation(float Damage, APRCharacter* Causer)
{
	OnDamaged.Broadcast(Damage, Causer);
}

void APRCharacter::MulticastDeath_Implementation()
{
	bIsDeath = true;
	SetCanBeDamaged(false);

	if (auto* MyController = GetController())
		MyController->UnPossess();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);

	OnDeath.Broadcast();
}

void APRCharacter::OnRep_IsLocked()
{
	Cast<UPRMovementComponent>(GetCharacterMovement())->ApplyLock(bIsLocked);
}

void APRCharacter::ApplyCharacterData(const FCharacterData& Data)
{
	GetCapsuleComponent()->SetCapsuleSize(Data.CapsuleRadius, Data.CapsuleHalfHeight);

	GetMesh()->SetAnimClass(Data.AnimClass);
	GetMesh()->SetSkeletalMesh(Data.Mesh.Get());

	const FVector& Location = GetMesh()->GetRelativeLocation();
	const FRotator& Rotation = GetMesh()->GetRelativeRotation();

	GetMesh()->SetRelativeLocationAndRotation(
		FVector{ Location.X, Location.Y, Data.MeshZ },
		FRotator{ Rotation.Pitch, Data.MeshYaw, Rotation.Roll }
	);

	RightWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("weapon_r"));
	LeftWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("weapon_l"));
}
