// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectRGameInstance.h"
#include "Weapon.h"
#include "WeaponData.h"

UWeaponComponent::UWeaponComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = false;

	RightWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Right Weapon"));
	RightWeapon->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	RightWeapon->SetCollisionProfileName(TEXT("Weapon"));
	RightWeapon->SetGenerateOverlapEvents(false);

	LeftWeapon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Left Weapon"));
	LeftWeapon->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	LeftWeapon->SetCollisionProfileName(TEXT("Weapon"));
	LeftWeapon->SetGenerateOverlapEvents(false);

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

	auto* MeshComponent = Cast<ACharacter>(GetOwner())->GetMesh();
	RightWeapon->SetupAttachment(MeshComponent, TEXT("weapon_r"));
	LeftWeapon->SetupAttachment(MeshComponent, TEXT("weapon_l"));
}

void UWeaponComponent::EquipWeapon(UWeapon* NewWeapon)
{
	if (CurWeapon)
		CurWeapon->Unequip();

	CurWeapon = NewWeapon;
	if (!CurWeapon) return;

	CurWeapon->Equip();

	RightWeapon->SetStaticMesh(CurWeapon->GetRightWeaponMesh());
	RightWeapon->SetRelativeTransform(CurWeapon->GetRightWeaponTransform());

	LeftWeapon->SetStaticMesh(CurWeapon->GetLeftWeaponMesh());
	LeftWeapon->SetRelativeTransform(CurWeapon->GetLeftWeaponTransform());
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
	FDetachmentTransformRules Rules = FDetachmentTransformRules{ EDetachmentRule::KeepWorld, true };

	LeftWeapon->SetCollisionProfileName(TEXT("Ragdoll"));
	LeftWeapon->SetSimulatePhysics(true);
	LeftWeapon->DetachFromComponent(Rules);

	RightWeapon->SetCollisionProfileName(TEXT("Ragdoll"));
	RightWeapon->SetSimulatePhysics(true);
	RightWeapon->DetachFromComponent(Rules);
}
