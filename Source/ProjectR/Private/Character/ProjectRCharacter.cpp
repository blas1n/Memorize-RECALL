// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ProjectRCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Buff/Cast.h"
#include "Buff/Lock.h"
#include "Buff/Parry.h"
#include "Buff/Run.h"
#include "Character/ProjectRPlayerState.h"
#include "Data/CharacterData.h"
#include "BuffLibrary.h"
#include "Parryable.h"
#include "ProjectRStatics.h"
#include "WeaponComponent.h"

AProjectRCharacter::AProjectRCharacter()
	: Super()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Hitable"));

	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.1f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_CharacterData.DT_CharacterData'"));
	CharacterDataTable = DataTable.Object;

	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon"));
	bIsDeath = false;

	OnAttack.AddDynamic(this, &AProjectRCharacter::HealHealthAndEnergy);
	Initialize();
}

void AProjectRCharacter::Attack(AProjectRCharacter* Target, int32 Damage)
{
	if (Target->IsPlayerControlled() == IsPlayerControlled())
		return;

	auto TakingDamage = static_cast<int32>(Target->
		TakeDamage(Damage, FDamageEvent{}, GetController(), this));

	if (TakingDamage > 0u)
		OnAttack.Broadcast(Target, TakingDamage);
}

#if WITH_EDITOR

void AProjectRCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!PropertyChangedEvent.Property) return;

	const FName& Name = PropertyChangedEvent.GetPropertyName();
	if (Name == TEXT("Level") || Name == TEXT("WeaponKeies"))
		WeaponComponent->Initialize(WeaponKeies);
	else
		Initialize();
}

#endif

void AProjectRCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Initialize();
}

float AProjectRCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	auto Damage = static_cast<int32>(DamageAmount);
	auto* Character = Cast<AProjectRCharacter>(DamageCauser);

	if (UBuffLibrary::GetBuff<UParry>(this)->ParryIfCan(Damage, EventInstigator, Character))
		return 0.0f;

	Damage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	auto* MyPlayerState = GetPlayerState<AProjectRPlayerState>();
	MyPlayerState->HealHealth(-Damage);

	if (MyPlayerState->GetHealth() == 0u) Death();
	else OnDamaged.Broadcast(EventInstigator, Damage);

	return Damage;
}

void AProjectRCharacter::Landed(const FHitResult& Hit)
{
	OnLand.Broadcast(Hit);
}

void AProjectRCharacter::Initialize()
{
	const auto* Data = CharacterDataTable->FindRow<FCharacterData>(FName{ *FString::FromInt(Key) }, TEXT(""));
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found character data %d!"), Key);
		return;
	}

	UProjectRStatics::AsyncLoad(Data->Mesh, [this, Data]() mutable
	{
		static const auto Rules = FAttachmentTransformRules::KeepRelativeTransform;

		GetCapsuleComponent()->SetCapsuleHalfHeight(Data->CapsuleHalfHeight);
		GetCapsuleComponent()->SetCapsuleRadius(Data->CapsuleRadius);

		GetMesh()->SetSkeletalMesh(Data->Mesh.Get());
		GetMesh()->SetAnimClass(Data->AnimClass);

		if (GetMesh()->DoesSocketExist(TEXT("weapon_r")))
			WeaponComponent->GetRightWeapon()->AttachToComponent(GetMesh(), Rules, TEXT("weapon_r"));
		if (GetMesh()->DoesSocketExist(TEXT("weapon_l")))
			WeaponComponent->GetLeftWeapon()->AttachToComponent(GetMesh(), Rules, TEXT("weapon_l"));

		const FVector& Location = GetMesh()->GetRelativeLocation();
		GetMesh()->SetRelativeLocation(FVector{ Location.X, Location.Y, Data->MeshZ });

		const FRotator& Rotation = GetMesh()->GetRelativeRotation();
		GetMesh()->SetRelativeRotation(FRotator{ Rotation.Pitch, Data->MeshYaw, Rotation.Roll });
	});

	WeaponComponent->Initialize(WeaponKeies);
}

void AProjectRCharacter::HealHealthAndEnergy(AProjectRCharacter* Target, int32 Damage)
{
	auto* MyPlayerState = GetPlayerState<AProjectRPlayerState>();
	if (!MyPlayerState) return;

	MyPlayerState->HealHealthByDamage(Damage);
	MyPlayerState->HealEnergyByDamage(Damage);
}

FVector AProjectRCharacter::GetViewLocation_Implementation() const
{
	FVector Vec;
	FRotator Rot;
	GetActorEyesViewPoint(Vec, Rot);
	return Vec;
}

void AProjectRCharacter::Death()
{
	UBuffLibrary::GetBuff<UCast>(this)->StopCast();

	bIsDeath = true;
	StopAnimMontage();
	OnDeath.Broadcast(LastHitBy);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);
}
