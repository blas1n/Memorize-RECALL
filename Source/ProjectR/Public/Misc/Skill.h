// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Skill.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTR_API USkill final : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void Begin(class USkillContext* Context, class UDataAsset* Data);

	UFUNCTION(BlueprintCallable)
	void End();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanUseSkill() const;

	UWorld* GetWorld() const override;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Begin"))
	void ReceiveBegin(USkillContext* Context, UDataAsset* Data);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "End"))
	void ReceiveEnd();

private:
	bool CanUseSkill_Implementation() const noexcept { return true; }

private:
	UPROPERTY(Transient, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	class APRCharacter* User;
};
