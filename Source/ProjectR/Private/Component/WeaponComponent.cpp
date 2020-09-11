// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/WeaponComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Component/WeaponMeshComponent.h"
#include "Framework/PRCharacter.h"
#include "Framework/PRAnimInstance.h"
#include "Misc/SkillContext.h"
#include "Misc/Weapon.h"

UWeaponComponent::UWeaponComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

	WeaponSwapDuration = 0.2f;
}

void UWeaponComponent::SetComponents(const TArray<UPrimitiveComponent*>& InComponents)
{
	Components = InComponents;

	if (HasBegunPlay())
		SkillContext->Initialize(Components);
}

void UWeaponComponent::Attack(bool bIsStrongAttack)
{
	ServerAttack(bIsStrongAttack);
}

void UWeaponComponent::Dodge()
{
	ServerDodge();
}

void UWeaponComponent::StopSkill()
{
	ServerStopSkill();
}

void UWeaponComponent::SwapWeapon(uint8 Index)
{
	ServerSwapWeapon(Index);
}

void UWeaponComponent::ChangeWeapon(uint8 Index, int32 Key)
{
	ServerChangeWeapon(Index, Key);
}

void UWeaponComponent::AddWeapon(int32 Key)
{
	ServerAddWeapon(Key);
}

void UWeaponComponent::Execute()
{
	if (GetOwner()->HasAuthority() && Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->Execute(bNowDodge ? 0u : SkillIndex + 1);
}

void UWeaponComponent::BeginExecute()
{
	if (GetOwner()->HasAuthority() && Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->BeginExecute(bNowDodge ? 0u : SkillIndex + 1);
}

void UWeaponComponent::EndExecute()
{
	if (GetOwner()->HasAuthority() && Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->EndExecute(bNowDodge ? 0u : SkillIndex + 1);
}

void UWeaponComponent::EnableCombo()
{
	if (Cast<APawn>(GetOwner())->HasAuthority()
		&& Weapons.IsValidIndex(WeaponIndex))
	{
		bNowCombo = true;
	}
}

void UWeaponComponent::DisableCombo()
{
	if (Cast<APawn>(GetOwner())->HasAuthority()
		&& Weapons.IsValidIndex(WeaponIndex))
	{
		bNowCombo = false;
	}
}

void UWeaponComponent::OnEndSkill()
{
	if (!bNowCombo)
		SkillIndex = 255u;

	bIsCasting = bNowDodge = false;
}

void UWeaponComponent::SetWeaponComponent(UWeaponMeshComponent* InRightWeapon,
	UWeaponMeshComponent* InLeftWeapon) noexcept
{
	RightWeapon = InRightWeapon;
	LeftWeapon = InLeftWeapon;
}

#if WITH_EDITOR

void UWeaponComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	static const FName& KeiesName = GET_MEMBER_NAME_CHECKED(UWeaponComponent, Keies);
	if (PropertyChangedEvent.Property && PropertyChangedEvent.GetPropertyName() == KeiesName)
		Initialize();
}

#endif

void UWeaponComponent::InitializeComponent()
{
	Super::InitializeComponent();
	Initialize();
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	auto* User = Cast<APRCharacter>(GetOwner());
	User->OnDeath.AddDynamic(this, &UWeaponComponent::Detach);

	if (GetOwnerRole() != ENetRole::ROLE_Authority)
		return;
	
	for (UWeapon* Weapon : Weapons)
		Weapon->BeginPlay();
	
	SkillContext->Initialize(Components);
	SkillIndex = 255u;
}

void UWeaponComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
		for (UWeapon* Weapon : Weapons)
			Weapon->EndPlay();

	auto* User = Cast<APRCharacter>(GetOwner());
	User->OnDeath.RemoveDynamic(this, &UWeaponComponent::Detach);

	Super::EndPlay(EndPlayReason);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponComponent, VisualData);
}

void UWeaponComponent::EquipWeapon(UWeapon* NewWeapon)
{
	if (!NewWeapon) return;

	NewWeapon->RegisterOnAsyncLoadEnded(
		FOnAsyncLoadEndedSingle::CreateLambda([this, NewWeapon]
		{
			VisualData = NewWeapon->GetVisualData();
			OnRep_VisualData();
		}
	));
}

void UWeaponComponent::Initialize()
{
	if (GetOwnerRole() != ENetRole::ROLE_Authority)
		return;

	SkillContext = NewObject<USkillContext>(this);

	if (!NoWeapon) NoWeapon = NewObject<UWeapon>(GetOwner());
	NoWeapon->Initialize(SkillContext, 0u);

	Weapons.Empty();
	int32 WeaponNum = Keies.Num();
	for (int32 Idx = 0; Idx < WeaponNum; ++Idx)
	{
		UWeapon* Weapon = NewObject<UWeapon>(GetOwner());
		if (Weapon->Initialize(SkillContext, Keies[Idx]))
		{
			Weapons.Add(Weapon);
			continue;
		}
		
		Keies.RemoveAt(Idx);
		--WeaponNum;
		--Idx;
	}

	EquipWeapon(Weapons.Num() > 0 ? Weapons[0] : NoWeapon);
}

void UWeaponComponent::ServerAttack_Implementation(bool bIsStrongAttack)
{
	if ((bIsCasting && !bNowCombo) || !Weapons.IsValidIndex(WeaponIndex)) return;

	if (bNowCombo)
	{
		ServerStopSkill_Implementation();
		bNowCombo = false;
	}

	SkillIndex = SkillIndex != 255u ? (2u * SkillIndex) + 2u : 0u;
	if (bIsStrongAttack) ++SkillIndex;

	bIsCasting = true;
	Weapons[WeaponIndex]->BeginSkill(SkillIndex + 1u);
}

void UWeaponComponent::ServerDodge_Implementation()
{
	if (bNowDodge || (bIsCasting && !bNowCombo) || !Weapons.IsValidIndex(WeaponIndex)) return;

	if (bNowCombo)
	{
		ServerStopSkill_Implementation();
		bNowCombo = false;
	}

	bIsCasting = bNowDodge = true;
	Weapons[WeaponIndex]->BeginSkill(0u);
}

void UWeaponComponent::ServerStopSkill_Implementation()
{
	if (Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->EndSkill(bNowDodge ? 0u : SkillIndex + 1u);
}

void UWeaponComponent::ServerSwapWeapon_Implementation(uint8 Index)
{
	if ((bIsCasting && !bNowCombo) || WeaponIndex == Index)
		return;

	EquipWeapon(Weapons[Index]);
	WeaponIndex = Index;
}

void UWeaponComponent::ServerChangeWeapon_Implementation(uint8 Index, int32 Key)
{
	if (bIsCasting || !Weapons.IsValidIndex(Index) || Weapons[Index]->GetKey() == Key)
		return;

	auto* NewWeapon = NewObject<UWeapon>(GetOwner());
	NewWeapon->Initialize(SkillContext, Key);

	if (Index == WeaponIndex)
		EquipWeapon(NewWeapon);

	Weapons[Index] = NewWeapon;
}

void UWeaponComponent::ServerAddWeapon_Implementation(int32 Key)
{
	if (bIsCasting) return;

	UWeapon* NewWeapon = NoWeapon;
	
	if (Key != 0)
	{
		NewWeapon = NewObject<UWeapon>(GetOwner());
		NewWeapon->Initialize(SkillContext, Key);
	}

	if (Weapons.Num() == 0)
		EquipWeapon(NewWeapon);

	Weapons.Add(NewWeapon);
}

void UWeaponComponent::ServerSetLevel_Implementation(uint8 InLevel)
{
	Level = InLevel;
	for (auto* Weapon : Weapons)
		Weapon->InitSkill(Level);
}

void UWeaponComponent::OnRep_VisualData()
{
	RightWeapon->SetWeapon(VisualData.RightMesh, VisualData.RightAnim, VisualData.RightTransform);
	LeftWeapon->SetWeapon(VisualData.LeftMesh, VisualData.LeftAnim, VisualData.LeftTransform);
}

void UWeaponComponent::Detach()
{
	RightWeapon->Detach();
	LeftWeapon->Detach();
}
