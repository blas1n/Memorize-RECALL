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

	FORCEINLINE class UWeapon* GetWeapon() const noexcept { return CurWeapon; }
	FORCEINLINE uint8 GetWeaponIndex() const noexcept { return CurIndex; }

private:
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason);

	void EquipWeapon(UWeapon* NewWeapon);


	UFUNCTION()
	void OnWeaponOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EnableRagdoll(AController* Instigator);

	class AStaticMeshActor* CreateWeaponActor(FName Socket);
	void DetachWeapon(AStaticMeshActor* Weapon);

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
