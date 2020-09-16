// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Skill.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTR_API USkill : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Initialize();
	virtual void Begin(class USkillContext* Context, const class UDataAsset* Data);
	virtual void Tick(float DeltaTime);
	virtual void End();

	UFUNCTION(BlueprintNativeEvent)
	bool CanUseSkill() const;

	UWorld* GetWorld() const override;

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Initialize"))
	void ReceiveInitialize();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Begin"))
	void ReceiveBegin(USkillContext* Context, const UDataAsset* Data);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Tick"))
	void ReceiveTick(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "End"))
	void ReceiveEnd();

	UFUNCTION(BlueprintCallable, meta = (BlueprintProtected))
	void Finish();

	virtual bool CanUseSkill_Implementation() const { return true; }

	FORCEINLINE class APRCharacter* GetUser() const noexcept { return User; }

private:
	UPROPERTY(Transient, BlueprintReadOnly, Category = Owner, meta = (AllowPrivateAccess = true))
	APRCharacter* User;
};
