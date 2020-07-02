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
	User = nullptr;
	CurWeapon = nullptr;
	WeaponNum = 0;
	CurIndex = 0;
}

void UWeaponComponent::UseSkill(uint8 Index)
{
	if (!User->IsCasting() && CurWeapon)
		CurWeapon->UseSkill(Index);
}

bool UWeaponComponent::CanUseSkill(uint8 Index)
{
	if (User->IsCasting() || !CurWeapon) return false;
	return CurWeapon->CanUseSkill(Index);
}

void UWeaponComponent::SwapWeapon(uint8 Index)
{
	if (User->IsCasting() || CurIndex == Index || Weapons[Index] == nullptr)
		return;
	
	EquipWeapon(Weapons[Index]);
	CurIndex = Index;
}

void UWeaponComponent::SetNewWeapon(FName Name, uint8 Index)
{
	check(Index < 3);

	if (!Weapons[Index]) ++WeaponNum;

	Weapons[Index] = NewObject<UWeapon>(GetOwner());
	Weapons[Index]->Initialize(Name);

	if (CurIndex == Index)
		EquipWeapon(Weapons[Index]);
}

void UWeaponComponent::SetWeaponCollision(bool bEnableRight, bool bEnableLeft)
{
	RightWeapon->SetGenerateOverlapEvents(bEnableRight);
	LeftWeapon->SetGenerateOverlapEvents(bEnableLeft);
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	User = Cast<AProjectRCharacter>(GetOwner());
	User->OnDeath.AddDynamic(this, &UWeaponComponent::EnableRagdoll);

	FActorSpawnParameters Param;
	Param.Owner = Param.Instigator = Cast<APawn>(GetOwner());

	RightWeapon = CreateWeaponMesh(TEXT("weapon_r"));
	LeftWeapon = CreateWeaponMesh(TEXT("weapon_l"));
}

void UWeaponComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	for (UWeapon* Weapon : Weapons)
		if (Weapon)	Weapon->Release();

	Super::EndPlay(EndPlayReason);
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
	RightWeapon->SetStaticMesh(CurWeapon->GetRightWeaponMesh());
	RightWeapon->SetRelativeTransform(CurWeapon->GetRightWeaponTransform());

	LeftWeapon->SetStaticMesh(CurWeapon->GetLeftWeaponMesh());
	LeftWeapon->SetRelativeTransform(CurWeapon->GetLeftWeaponTransform());
}

void UWeaponComponent::OnWeaponOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CurWeapon->OnWeaponHitted.Broadcast(Cast<AProjectRCharacter>(OtherActor));
}

void UWeaponComponent::EnableRagdoll(AController* Instigator)
{
	DetachWeapon(RightWeapon);
	DetachWeapon(LeftWeapon);
}

void UWeaponComponent::DetachWeapon(UStaticMeshComponent* Weapon)
{
	Weapon->SetCollisionProfileName(TEXT("Ragdoll"));
	Weapon->SetSimulatePhysics(true);

	auto Rules = FDetachmentTransformRules::KeepWorldTransform;
	Weapon->DetachFromComponent(Rules);
}

UStaticMeshComponent* UWeaponComponent::CreateWeaponMesh(FName Socket)
{
	auto* Component = NewObject<UStaticMeshComponent>(this);
	Component->RegisterComponent();
	Component->SetMobility(EComponentMobility::Movable);
	Component->OnComponentBeginOverlap.AddDynamic(this, &UWeaponComponent::OnWeaponOverlapped);

	auto* MeshComponent = Cast<ACharacter>(GetOwner())->GetMesh();
	const auto Rules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
	Component->AttachToComponent(MeshComponent, Rules, Socket);
	
	Component->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	Component->SetCollisionProfileName(TEXT("Weapon"));
	Component->SetGenerateOverlapEvents(false);

	return Component;
}
