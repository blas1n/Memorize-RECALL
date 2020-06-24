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

	void SetWeaponCollision(bool bEnableRight, bool bEnableLeft);

	FORCEINLINE class UWeapon* GetWeapon() const noexcept { return CurWeapon; }
	FORCEINLINE uint8 GetWeaponIndex() const noexcept { return CurIndex; }

private:
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason);

	void EquipWeapon(UWeapon* NewWeapon);

	UFUNCTION()
	void SetWeaponMesh();

	UFUNCTION()
	void OnWeaponOverlapped(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void EnableRagdoll(AController* Instigator);

	void DetachWeapon(class AStaticMeshActor* Weapon);
	AStaticMeshActor* CreateWeaponActor(FName Socket);

private:
	UPROPERTY()
	AStaticMeshActor* RightWeapon;

	UPROPERTY()
	AStaticMeshActor* LeftWeapon;

	UPROPERTY()
	TArray<UWeapon*> Weapons;

	UPROPERTY()
	UWeapon* CurWeapon;

	uint8 CurIndex;
};
