// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectRCharacter.h"
#include "Weapon.h"

UWeaponComponent::UWeaponComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = false;

	RightWeapon = nullptr;
	LeftWeapon = nullptr;
	Weapons.Init(nullptr, 3);
	CurWeapon = nullptr;
	CurIndex = 0;
}

void UWeaponComponent::UseSkill(uint8 Index)
{
	if (CurWeapon)
		CurWeapon->UseSkill(Index);
}

bool UWeaponComponent::CanUseSkill(uint8 Index)
{
	if (!CurWeapon) return false;
	return CurWeapon->CanUseSkill(Index);
}

void UWeaponComponent::SwapWeapon(uint8 Index)
{
	if (CurIndex != Index)
		EquipWeapon(Weapons[Index]);
}

void UWeaponComponent::SetNewWeapon(FName Name, uint8 Index)
{
	check(Index < 3);

	Weapons[Index] = NewObject<UWeapon>(GetOwner());
	Weapons[Index]->Initialize(Name);

	if (CurIndex == Index)
		EquipWeapon(Weapons[Index]);
}

void UWeaponComponent::SetWeaponCollision(bool bEnableRight, bool bEnableLeft)
{
	RightWeapon->GetStaticMeshComponent()->SetGenerateOverlapEvents(bEnableRight);
	LeftWeapon->GetStaticMeshComponent()->SetGenerateOverlapEvents(bEnableLeft);
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	auto* User = Cast<AProjectRCharacter>(GetOwner());
	User->OnDeath.AddDynamic(this, &UWeaponComponent::EnableRagdoll);

	FActorSpawnParameters Param;
	Param.Owner = Param.Instigator = Cast<APawn>(GetOwner());

	RightWeapon = CreateWeaponActor(TEXT("weapon_r"));
	LeftWeapon = CreateWeaponActor(TEXT("weapon_l"));
}

void UWeaponComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	RightWeapon->Destroy();
	LeftWeapon->Destroy();
}

void UWeaponComponent::EquipWeapon(UWeapon* NewWeapon)
{
	if (CurWeapon)
		CurWeapon->Unequip();

	CurWeapon = NewWeapon;
	if (!CurWeapon) return;

	CurWeapon->Equip();

	FOnAsyncLoadEndedSingle Callback;
	Callback.BindDynamic(this, &UWeaponComponent::SetWeaponMesh);
	CurWeapon->RegisterOnAsyncLoadEnded(Callback);
}

void UWeaponComponent::SetWeaponMesh()
{
	RightWeapon->GetStaticMeshComponent()->SetStaticMesh(CurWeapon->GetRightWeaponMesh());
	RightWeapon->SetActorRelativeTransform(CurWeapon->GetRightWeaponTransform());

	LeftWeapon->GetStaticMeshComponent()->SetStaticMesh(CurWeapon->GetLeftWeaponMesh());
	LeftWeapon->SetActorRelativeTransform(CurWeapon->GetLeftWeaponTransform());
}

void UWeaponComponent::OnWeaponOverlapped(AActor* OverlappedActor, AActor* OtherActor)
{
	CurWeapon->OnWeaponHitted.Broadcast(Cast<AProjectRCharacter>(OtherActor));
}

void UWeaponComponent::EnableRagdoll(AController* Instigator)
{
	DetachWeapon(RightWeapon);
	DetachWeapon(LeftWeapon);
}

void UWeaponComponent::DetachWeapon(AStaticMeshActor* Weapon)
{
	auto Component = Weapon->GetStaticMeshComponent();
	Component->SetCollisionProfileName(TEXT("Ragdoll"));
	Component->SetSimulatePhysics(true);

	auto Rules = FDetachmentTransformRules::KeepWorldTransform;
	Weapon->DetachFromActor(Rules);
}

AStaticMeshActor* UWeaponComponent::CreateWeaponActor(FName Socket)
{
	FActorSpawnParameters Param;
	Param.Owner = Param.Instigator = Cast<APawn>(GetOwner());
	auto* WeaponActor = GetWorld()->SpawnActor<AStaticMeshActor>(Param);
	WeaponActor->SetMobility(EComponentMobility::Movable);
	WeaponActor->OnActorBeginOverlap.AddDynamic(this, &UWeaponComponent::OnWeaponOverlapped);

	auto* MeshComponent = Cast<ACharacter>(GetOwner())->GetMesh();
	auto Rules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
	WeaponActor->AttachToComponent(MeshComponent, Rules, Socket);
	
	auto* WeaponComponent = WeaponActor->GetStaticMeshComponent();
	WeaponComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	WeaponComponent->SetCollisionProfileName(TEXT("Weapon"));
	WeaponComponent->SetGenerateOverlapEvents(false);

	return WeaponActor;
}
