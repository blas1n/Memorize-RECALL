// Fill out your copyright notice in the Description page of Project Settings.

#include "Skill.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Buff/Cast.h"
#include "Character/ProjectRCharacter.h"
#include "Character/ProjectRPlayerState.h"
#include "Data/SkillData.h"
#include "Library/BuffLibrary.h"
#include "Weapon.h"

USkill::USkill()
	: Super()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTable(TEXT("DataTable'/Game/Data/DataTable/DT_SkillData.DT_SkillData'"));
	SkillDataTable = DataTable.Object;
}

void USkill::Initialize(const FName& KeyStr)
{
	Weapon = GetTypedOuter<UWeapon>();
	if (!Weapon) return;

	User = Cast<AProjectRCharacter>(Weapon->GetOuter());
	if (!User) return;

	const auto* Data = SkillDataTable->FindRow<FSkillData>(KeyStr, TEXT(""));
	if (!Data)
	{
		UE_LOG(LogDataTable, Error, TEXT("Cannot found skill data %s!"), *KeyStr.ToString());
		return;
	}

	Priority = Data->Priority;
	CoolTime = Data->CoolTime;
	UseEnergy = Data->UseEnergy;

	ReceiveInitialize(Data->AdditionalData);
}

void USkill::BeginPlay()
{
	ReceiveBeginPlay();
}

void USkill::Start()
{
	if (GetPriority() >= 0)
	{
		auto* Cast = UBuffLibrary::GetBuff<UCast>(User);
		Cast->SetCastSkill(this);
		Cast->Apply();
	}

	ReceiveStart();
}

void USkill::End()
{
	if (GetPriority() >= 0)
		UBuffLibrary::ReleaseBuff<UCast>(User);

	ReceiveEnd();
}

UWorld* USkill::GetWorld() const
{
	return User ? User->GetWorld() : nullptr;
}

bool USkill::IsNotCoolTime() const
{
	return FMath::IsNearlyEqual(NextUseTime, 0.0f) || NextUseTime <= GetWorld()->GetTimeSeconds();
}

void USkill::ApplyCooltime()
{
	NextUseTime = GetWorld()->GetTimeSeconds() + CoolTime;
}

bool USkill::IsEnoughEnergy() const
{
	auto* PlayerState = User->GetPlayerState<AProjectRPlayerState>();
	if (!PlayerState) return true;

	return PlayerState->GetEnergy() >= UseEnergy;
}

void USkill::ApplyEnergy()
{
	if (auto* PlayerState = User->GetPlayerState<AProjectRPlayerState>())
		PlayerState->HealEnergy(-UseEnergy);
}

bool USkill::CanUse_Implementation() const
{
	return UBuffLibrary::GetBuff<UCast>(User)->CanUseSkill(this);
}
