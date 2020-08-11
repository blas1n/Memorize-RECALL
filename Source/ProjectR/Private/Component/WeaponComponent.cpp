// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/WeaponComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "Framework/PRCharacter.h"
#include "Library/BuffLibrary.h"
#include "Weapon/SkillContext.h"
#include "Weapon/Weapon.h"

UWeaponComponent::UWeaponComponent()
	: Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

	RightWeapon = CreateWeaponComponent(TEXT("RightWeapon"));
	LeftWeapon = CreateWeaponComponent(TEXT("LeftWeapon"));
}

void UWeaponComponent::Attack(bool bIsStrongAttack)
{
	if (bCheckCombo)
	{
		NextCombo = bIsStrongAttack ? ENextCombo::Strong : ENextCombo::Week;
		bCheckCombo = false;
	}
	else if (!bIsCasting && NextCombo == ENextCombo::None)
	{
		ServerAttack(bIsStrongAttack, false);
		bIsCasting = true;
	}
}

void UWeaponComponent::Parry()
{
	if (!bIsCasting)
	{
		ServerParry();
		bIsCasting = true;
	}
}

void UWeaponComponent::StopSkill()
{
	if (bIsCasting) ServerStopSkill();
}

void UWeaponComponent::SwapWeapon(uint8 Index)
{
	if (!bIsCasting) ServerSwapWeapon(Index);
}

void UWeaponComponent::AddWeapon(uint8 Index, int32 Key)
{
	if (!bIsCasting) ServerAddWeapon(Index, Key);
}

void UWeaponComponent::Execute()
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority && Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->Execute(bIsParrying ? 0u : SkillIndex + 1);
}

void UWeaponComponent::BeginExecute()
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority && Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->BeginExecute(bIsParrying ? 0u : SkillIndex + 1);
}

void UWeaponComponent::EndExecute()
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority && Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->EndExecute(bIsParrying ? 0u : SkillIndex + 1);
}

void UWeaponComponent::CheckCombo()
{
	if (GetOwnerRole() == ENetRole::ROLE_AutonomousProxy)
		bCheckCombo = true;
}

void UWeaponComponent::ExecuteCombo()
{
	bCheckCombo = false;
	if (NextCombo != ENextCombo::None && GetOwnerRole() == ENetRole::ROLE_AutonomousProxy)
		ServerAttack(NextCombo == ENextCombo::Strong ? true : false, true);
}

void UWeaponComponent::OnEndSkill()
{
	if (NextCombo != ENextCombo::None)
		SkillIndex = 0u;
	else
		NextCombo = ENextCombo::None;

	ClientOnStopSkill();
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

	for (UWeapon* Weapon : Weapons)
		Weapon->BeginPlay();

	if (Weapons.Num() > 0)
		EquipWeapon(Weapons[0], false);
}

void UWeaponComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	for (UWeapon* Weapon : Weapons)
		Weapon->EndPlay();

	auto* User = Cast<APRCharacter>(GetOwner());
	User->OnDeath.RemoveDynamic(this, &UWeaponComponent::Detach);

	Super::EndPlay(EndPlayReason);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponComponent, SkillContext);
	DOREPLIFETIME(UWeaponComponent, VisualData);
}

UStaticMeshComponent* UWeaponComponent::CreateWeaponComponent(const FName& Name)
{
	auto* Component = CreateDefaultSubobject<UStaticMeshComponent>(Name);
	Component->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	Component->SetMobility(EComponentMobility::Movable);
	Component->SetCollisionProfileName(TEXT("Weapon"));
	Component->SetGenerateOverlapEvents(true);
	return Component;
}

void UWeaponComponent::EquipWeapon(UWeapon* NewWeapon, bool bNeedUnequip)
{
	if (!NewWeapon) return;

	if (bNeedUnequip)
		Weapons[WeaponIndex]->Unequip();

	NewWeapon->Equip();
}

void UWeaponComponent::Initialize()
{
	auto* MeshComponent = CastChecked<ACharacter>(GetOwner())->GetMesh();
	const auto Rules = FAttachmentTransformRules::KeepRelativeTransform;

	if (MeshComponent->DoesSocketExist(TEXT("weapon_r")))
		RightWeapon->AttachToComponent(MeshComponent, Rules, TEXT("weapon_r"));

	if (MeshComponent->DoesSocketExist(TEXT("weapon_l")))
		LeftWeapon->AttachToComponent(MeshComponent, Rules, TEXT("weapon_l"));
	
	SkillContext = NewObject<USkillContext>(this);
	SkillContext->Initialize(RightWeapon, LeftWeapon);

	if (GetOwnerRole() != ENetRole::ROLE_Authority)
		return;

	if (!NoWeapon) NoWeapon = NewObject<UWeapon>(GetOwner());
	NoWeapon->Initialize(SkillContext, 0);

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
}

void UWeaponComponent::ServerAttack_Implementation(bool bIsStrongAttack, bool bIsCombo)
{
	if (!Weapons.IsValidIndex(WeaponIndex)) return;
	UWeapon* Weapon = Weapons[WeaponIndex];

	if (bIsCombo)
	{
		NextCombo = bIsStrongAttack ? ENextCombo::Strong : ENextCombo::Week;
		ServerStopSkill_Implementation();
		SkillIndex = (2u * SkillIndex) + 2u;
	}

	if (bIsStrongAttack) ++SkillIndex;
	Weapon->BeginSkill(SkillIndex + 1);
}

bool UWeaponComponent::ServerAttack_Validate(bool bIsStrongAttack, bool bIsCombo)
{
	return true;
}

void UWeaponComponent::ServerParry_Implementation()
{
	if (Weapons.IsValidIndex(WeaponIndex))
	{
		Weapons[WeaponIndex]->BeginSkill(0u);
		bIsParrying = true;
	}
}

bool UWeaponComponent::ServerParry_Validate()
{
	return true;
}

void UWeaponComponent::ServerStopSkill_Implementation()
{
	if (Weapons.IsValidIndex(WeaponIndex))
	{
		Weapons[WeaponIndex]->EndSkill(bIsParrying ? 0u : SkillIndex + 1);
		bIsParrying = false;
	}
}

bool UWeaponComponent::ServerStopSkill_Validate()
{
	return true;
}

void UWeaponComponent::ServerSwapWeapon_Implementation(uint8 Index)
{
	if (WeaponIndex == Index || bIsCasting)
		return;

	EquipWeapon(Weapons[Index], true);
	WeaponIndex = Index;
}

bool UWeaponComponent::ServerSwapWeapon_Validate(uint8 Index)
{
	return Weapons.Num() > Index;
}

void UWeaponComponent::ServerAddWeapon_Implementation(uint8 Index, int32 Key)
{
	const int32 BeforeWeaponNum = Weapons.Num();
	if (Index > BeforeWeaponNum)
		Weapons.SetNum(Index + 1u);

	const int32 AfterWeaponNum = Weapons.Num();
	for (int32 Idx = BeforeWeaponNum; Idx < AfterWeaponNum; ++Idx)
		Weapons[Idx] = NoWeapon;

	auto* NewWeapon = NewObject<UWeapon>(GetOwner());
	NewWeapon->Initialize(SkillContext, Key);

	if (Index == WeaponIndex)
		EquipWeapon(NewWeapon, true);

	Weapons[Index] = NewWeapon;
}

bool UWeaponComponent::ServerAddWeapon_Validate(uint8 Index, int32 Key)
{
	return true;
}

void UWeaponComponent::ClientOnStopSkill_Implementation()
{
	NextCombo = ENextCombo::None;
	bCheckCombo = false;
	bIsCasting = false;
}

void UWeaponComponent::Detach()
{
	DetachOnce(RightWeapon);
	DetachOnce(LeftWeapon);
}

void UWeaponComponent::DetachOnce(UStaticMeshComponent* Weapon)
{
	Weapon->SetCollisionProfileName(TEXT("Ragdoll"));
	Weapon->SetSimulatePhysics(true);

	auto Rules = FDetachmentTransformRules::KeepWorldTransform;
	Weapon->DetachFromComponent(Rules);
}
