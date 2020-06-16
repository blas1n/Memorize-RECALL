// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectRCharacter.h"
#include "ProjectRGameInstance.h"
#include "Weapon.h"
#include "WeaponData.h"

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
	Weapons[Index] = CreateWeapon(Name);
	if (CurIndex == Index)
		EquipWeapon(Weapons[Index]);
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

	RightWeapon->GetStaticMeshComponent()->SetStaticMesh(CurWeapon->GetRightWeaponMesh());
	RightWeapon->SetActorRelativeTransform(CurWeapon->GetRightWeaponTransform());

	LeftWeapon->GetStaticMeshComponent()->SetStaticMesh(CurWeapon->GetLeftWeaponMesh());
	LeftWeapon->SetActorRelativeTransform(CurWeapon->GetLeftWeaponTransform());
}

UWeapon* UWeaponComponent::CreateWeapon(const FName& Name)
{
	UWeapon* Ret = NewObject<UWeapon>(GetOwner());

	const auto* GameInstance = Cast<UProjectRGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	const UDataTable* DataTable = GameInstance->GetDataTable(TEXT("WeaponData"));
	const FWeaponData* WeaponData = DataTable->FindRow<FWeaponData>(Name, "", false);
	Ret->Initialize(Name, *WeaponData);
	return Ret;
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

AStaticMeshActor* UWeaponComponent::CreateWeaponActor(FName Socket)
{
	FActorSpawnParameters Param;
	Param.Owner = Param.Instigator = Cast<APawn>(GetOwner());
	auto* WeaponActor = GetWorld()->SpawnActor<AStaticMeshActor>(Param);

	auto* MeshComponent = Cast<ACharacter>(GetOwner())->GetMesh();
	auto Rules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
	WeaponActor->AttachToComponent(MeshComponent, Rules, Socket);

	auto* WeaponComponent = WeaponActor->GetStaticMeshComponent();
	WeaponComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	WeaponComponent->SetCollisionProfileName(TEXT("Weapon"));
	WeaponComponent->SetGenerateOverlapEvents(false);

	return WeaponActor;
}

void UWeaponComponent::DetachWeapon(AStaticMeshActor* Weapon)
{
	auto Component = Weapon->GetStaticMeshComponent();
	Component->SetCollisionProfileName(TEXT("Ragdoll"));
	Component->SetSimulatePhysics(true);

	auto Rules = FDetachmentTransformRules::KeepWorldTransform;
	Weapon->DetachFromActor(Rules);
}
