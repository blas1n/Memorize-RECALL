// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NetworkObject.generated.h"

UCLASS(Abstract, Blueprintable)
class PROJECTR_API UNetworkObject : public UObject
{
	GENERATED_BODY()

public:
	FORCEINLINE bool IsSupportedForNetworking() const override { return true; }

	FORCEINLINE virtual bool ReplicateSubobjects(class AActorChannel* Channel,
		class FOutBunch* Bunch, struct FReplicationFlags* RepFlags)
	{
		return false;
	}

	int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;
};
