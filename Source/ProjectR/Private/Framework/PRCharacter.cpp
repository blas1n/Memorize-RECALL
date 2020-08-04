// Fill out your copyright notice in the Description page of Project Settings.

#include "Framework/PRCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "UObject/ConstructorHelpers.h"
#include "Buff/Cast.h"
#include "Buff/Lock.h"
#include "Buff/Parry.h"
#include "Buff/Run.h"
#include "Component/StatComponent.h"
#include "Component/WeaponComponent.h"
#include "Data/CharacterData.h"
#include "Data/PRDamageType.h"
#include "Interface/Parryable.h"
#include "Library/BuffLibrary.h"
#include "Library/PRStatics.h"

APRCharacter::APRCharacter()
	: Super()
{
	PrimaryActorTick.bCanEverTick = false;

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

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_CharacterData.DT_CharacterData'"));
	CharacterDataTable = DataTable.Object;

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat"));
	bIsDeath = false;
	
	Initialize();
}

void APRCharacter::SetTeamId(uint8 NewTeamId)
{
	TeamId = NewTeamId;
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

void APRCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Initialize();
	OnAttack.AddDynamic(this, &APRCharacter::Heal);
}

float APRCharacter::TakeDamage(float Damage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	auto* Parry = UBuffLibrary::GetBuff<UParry>(this);
	auto* Character = Cast<APRCharacter>(DamageCauser);
	const auto* DamageType = Cast<UProjectRDamageType>(DamageEvent.DamageTypeClass.GetDefaultObject());
	
	Damage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	if (Damage <= 0.0f) return Damage;

	if (DamageType->IsWeaponAttack() && Parry && Parry->ParryIfCan(Damage, EventInstigator, Character))
		return 0.0f;

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

void APRCharacter::Landed(const FHitResult& Hit)
{
	OnLand.Broadcast(Hit);
}

void APRCharacter::Initialize()
{
	const auto* Data = CharacterDataTable->FindRow<FCharacterData>(FName{ *FString::FromInt(CharacterKey) }, TEXT(""));
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found character data %d!"), CharacterKey);
		return;
	}

	UPRStatics::AsyncLoad(Data->Mesh, [this, Data]() mutable
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
	});
}

void APRCharacter::Heal(float Damage, AActor* Target, TSubclassOf<UDamageType> DamageType)
{
	auto* DamageTypeObj = Cast<UProjectRDamageType>(DamageType.GetDefaultObject());
	if (DamageTypeObj->CanHealByDamage())
		StatComponent->HealByDamage(Damage);
}

void APRCharacter::GetLookLocationAndRotation_Implementation(FVector& Location, FRotator& Rotation) const
{
	Super::GetActorEyesViewPoint(Location, Rotation);
}

void APRCharacter::Death()
{
	WeaponComponent->StopSkill();

	bIsDeath = true;
	StopAnimMontage();
	SetCanBeDamaged(false);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);

	GetController()->UnPossess();
	OnDeath.Broadcast();
}
