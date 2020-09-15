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
	SkillIndex = 255u;
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
		Weapons[WeaponIndex]->Execute(CombatState == ECombatState::Dodge ? 0u : SkillIndex + 1);
}

void UWeaponComponent::BeginExecute()
{
	if (GetOwner()->HasAuthority() && Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->BeginExecute(CombatState == ECombatState::Dodge ? 0u : SkillIndex + 1);
}

void UWeaponComponent::EndExecute()
{
	if (GetOwner()->HasAuthority() && Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->EndExecute(CombatState == ECombatState::Dodge ? 0u : SkillIndex + 1);
}

void UWeaponComponent::EnableCombo()
{
	if (GetOwner()->HasAuthority()
		&& Weapons.IsValidIndex(WeaponIndex))
	{
		bNowCombo = true;
		OnEnableCombo.Broadcast();
	}
}

void UWeaponComponent::DisableCombo()
{
	if (GetOwner()->HasAuthority()
		&& Weapons.IsValidIndex(WeaponIndex))
	{
		bNowCombo = false;
		OnDisableCombo.Broadcast();
	}
}

void UWeaponComponent::SetLevel(uint8 InLevel)
{
	ServerSetLevel(InLevel);
}

void UWeaponComponent::OnEndSkill()
{
	if (!bNowCombo)
		SkillIndex = 255u;

	CombatState = ECombatState::None;
	OnStopSkill.Broadcast();
}

void UWeaponComponent::SetWeaponComponent(UWeaponMeshComponent* InRightWeapon,
	UWeaponMeshComponent* InLeftWeapon) noexcept
{
	RightWeapon = InRightWeapon;
	LeftWeapon = InLeftWeapon;
}

void UWeaponComponent::SetComponents(const TArray<class UPrimitiveComponent*>& Components)
{
	SkillContext->Initialize(Components);
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

	if (GetOwnerRole() == ENetRole::ROLE_Authority)
		for (auto* Weapon : Weapons)
			Weapon->InitSkill(Level);
}

void UWeaponComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Cast<APRCharacter>(GetOwner())->OnDeath.
		RemoveDynamic(this, &UWeaponComponent::Detach);

	Super::EndPlay(EndPlayReason);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, VisualData);
	DOREPLIFETIME(UWeaponComponent, Level);
	DOREPLIFETIME(UWeaponComponent, bNowCombo);
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
	if (bBlockSkill || (CombatState != ECombatState::None &&
		!bNowCombo) || !Weapons.IsValidIndex(WeaponIndex)) return;

	if (bNowCombo)
	{
		ServerStopSkill_Implementation();
		bNowCombo = false;
	}

	SkillIndex = SkillIndex != 255u ? (2u * SkillIndex) + 2u : 0u;
	if (bIsStrongAttack) ++SkillIndex;

	CombatState = ECombatState::Attack;
	Weapons[WeaponIndex]->BeginSkill(SkillIndex + 1u);
	OnAttack.Broadcast();
}

void UWeaponComponent::ServerDodge_Implementation()
{
	if (bBlockSkill || CombatState == ECombatState::Dodge ||
		(CombatState != ECombatState::None && !bNowCombo) ||
		!Weapons.IsValidIndex(WeaponIndex)) return;

	if (bNowCombo)
	{
		ServerStopSkill_Implementation();
		bNowCombo = false;
	}
	
	CombatState = ECombatState::Dodge;
	Weapons[WeaponIndex]->BeginSkill(0u);
	OnDodge.Broadcast();
}

void UWeaponComponent::ServerStopSkill_Implementation()
{
	if (Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->EndSkill(CombatState == ECombatState::Dodge ? 0u : SkillIndex + 1u);
}

void UWeaponComponent::ServerSwapWeapon_Implementation(uint8 Index)
{
	if (bBlockSkill || (CombatState != ECombatState::None
		&& !bNowCombo) || WeaponIndex == Index) return;

	EquipWeapon(Weapons[Index]);
	WeaponIndex = Index;
}

void UWeaponComponent::ServerChangeWeapon_Implementation(uint8 Index, int32 Key)
{
	if (bBlockSkill || CombatState != ECombatState::None ||
		!Weapons.IsValidIndex(Index) || Weapons[Index]->GetKey() == Key)
		return;

	auto* NewWeapon = NewObject<UWeapon>(GetOwner());
	NewWeapon->Initialize(SkillContext, Key);
	NewWeapon->InitSkill(Level);

	if (Index == WeaponIndex)
		EquipWeapon(NewWeapon);

	Weapons[Index] = NewWeapon;
}

void UWeaponComponent::ServerAddWeapon_Implementation(int32 Key)
{
	if (bBlockSkill || CombatState != ECombatState::None) return;

	UWeapon* NewWeapon = NoWeapon;
	
	if (Key != 0)
	{
		NewWeapon = NewObject<UWeapon>(GetOwner());
		NewWeapon->Initialize(SkillContext, Key);
	}

	NewWeapon->InitSkill(Level);
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
