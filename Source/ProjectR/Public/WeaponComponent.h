// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTR_API UWeaponComponent final : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponComponent();

	UFUNCTION(BlueprintCallable)
	void UseSkill(uint8 Index);

	UFUNCTION(BlueprintCallable)
	bool CanUseSkill(uint8 Index);

	UFUNCTION(BlueprintCallable)
	void SwapWeapon(uint8 Index);

	UFUNCTION(BlueprintCallable)
	void SetNewWeapon(FName Name, uint8 Index);

private:
	void BeginPlay() override;
	class UWeapon* CreateWeapon(const FName& Name);
	void EquipWeapon(UWeapon* NewWeapon);
	void EnableRagdoll();

private:
	UPROPERTY()
	class UStaticMeshComponent* RightWeapon;

	UPROPERTY()
	class UStaticMeshComponent* LeftWeapon;

	UPROPERTY()
	TArray<UWeapon*> Weapons;

	UPROPERTY()
	UWeapon* CurWeapon;

	uint8 CurIndex;
};
