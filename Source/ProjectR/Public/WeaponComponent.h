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
	bool CanUseSkill(uint8 Index) const;

	UFUNCTION(BlueprintCallable)
	void SwapWeapon(uint8 Index);

	UFUNCTION(BlueprintCallable)
	void SetNewWeapon(FName Name, uint8 Index);

	void SetWeaponCollision(bool bEnableRight, bool bEnableLeft);

	FORCEINLINE class UWeapon* GetWeapon() const noexcept { return CurWeapon; }
	FORCEINLINE uint8 GetWeaponNum() const noexcept { return WeaponNum; }
	FORCEINLINE uint8 GetWeaponIndex() const noexcept { return CurIndex; }

private:
	void BeginPlay() override;
	void EndPlay(EEndPlayReason::Type EndPlayReason);

	void EquipWeapon(UWeapon* NewWeapon);

	UFUNCTION()
	void SetWeaponMesh();

	UFUNCTION()
	void OnWeaponOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void EnableRagdoll(AController* Instigator);

	void DetachWeapon(class UStaticMeshComponent* Weapon);
	UStaticMeshComponent* CreateWeaponMesh(FName Socket);

private:
	UPROPERTY()
	UStaticMeshComponent* RightWeapon;

	UPROPERTY()
	UStaticMeshComponent* LeftWeapon;

	UPROPERTY()
	TArray<UWeapon*> Weapons;

	UPROPERTY()
	class AProjectRCharacter* User;

	UPROPERTY()
	UWeapon* CurWeapon;

	uint8 WeaponNum;
	uint8 CurIndex;
};
