// Fill out your copyright notice in the Description page of Project Settings.

#include "Component/WeaponComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Framework/PRCharacter.h"
#include "Misc/SkillContext.h"
#include "Misc/Weapon.h"

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
	ServerAttack(bIsStrongAttack);
}

void UWeaponComponent::Parry()
{
	ServerParry();
}

void UWeaponComponent::StopSkill()
{
	ServerStopSkill();
}

void UWeaponComponent::SwapWeapon(uint8 Index)
{
	ServerSwapWeapon(Index);
}

void UWeaponComponent::AddWeapon(uint8 Index, int32 Key)
{
	ServerAddWeapon(Index, Key);
}

void UWeaponComponent::Execute()
{
	if (GetOwner()->HasAuthority() && Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->Execute(bNowParry ? 0u : SkillIndex + 1);
}

void UWeaponComponent::BeginExecute()
{
	if (GetOwner()->HasAuthority() && Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->BeginExecute(bNowParry ? 0u : SkillIndex + 1);
}

void UWeaponComponent::EndExecute()
{
	if (GetOwner()->HasAuthority() && Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->EndExecute(bNowParry ? 0u : SkillIndex + 1);
}

void UWeaponComponent::TickExecute(float DeltaSeconds)
{
	if (GetOwner()->HasAuthority() && Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->TickExecute(bNowParry ? 0u : SkillIndex + 1, DeltaSeconds);
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

	bIsCasting = bNowParry = false;
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
	{
		for (UWeapon* Weapon : Weapons)
			Weapon->BeginPlay();

		if (Weapons.Num() > 0)
			EquipWeapon(Weapons[0], false);

		SkillIndex = 255u;
	}
	else ApplyWeapon(nullptr);
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

	NewWeapon->RegisterOnAsyncLoadEnded(
		FOnAsyncLoadEndedSingle::CreateLambda([this, NewWeapon, bNeedUnequip]
		{
			VisualData = NewWeapon->GetVisualData();
			MulticastEquipWeapon(Weapons[WeaponIndex]->GetVisualData().UpperAnimInstance);
		}
	));
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

void UWeaponComponent::ServerParry_Implementation()
{
	if ((bIsCasting && !bNowCombo) || !Weapons.IsValidIndex(WeaponIndex)) return;

	if (bNowCombo)
	{
		ServerStopSkill_Implementation();
		bNowCombo = false;
	}

	bIsCasting = bNowParry = true;
	Weapons[WeaponIndex]->BeginSkill(0u);
}

void UWeaponComponent::ServerStopSkill_Implementation()
{
	if (Weapons.IsValidIndex(WeaponIndex))
		Weapons[WeaponIndex]->EndSkill(bNowParry ? 0u : SkillIndex + 1u);
}

void UWeaponComponent::ServerSwapWeapon_Implementation(uint8 Index)
{
	if ((bIsCasting && !bNowCombo) || WeaponIndex == Index)
		return;

	EquipWeapon(Weapons[Index], true);
	WeaponIndex = Index;
}

void UWeaponComponent::ServerAddWeapon_Implementation(uint8 Index, int32 Key)
{
	if (bIsCasting || (Weapons.IsValidIndex(Index) && Weapons[Index]->GetKey() == Key))
		return;

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

void UWeaponComponent::ApplyWeapon(TSubclassOf<UAnimInstance> UnlinkAnim)
{
	if (auto* AnimInstance = Cast<ACharacter>(GetOwner())->GetMesh()->GetAnimInstance())
	{
		if (UnlinkAnim.Get())
			AnimInstance->UnlinkAnimClassLayers(Weapons[WeaponIndex]->GetVisualData().UpperAnimInstance);

		AnimInstance->LinkAnimClassLayers(VisualData.UpperAnimInstance);
	}
	
	RightWeapon->SetStaticMesh(VisualData.RightMesh);
	RightWeapon->SetRelativeTransform(VisualData.RightTransform);

	LeftWeapon->SetStaticMesh(VisualData.LeftMesh);
	LeftWeapon->SetRelativeTransform(VisualData.LeftTransform);
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
