// Fill out your copyright notice in the Description page of Project Settings.

#include "NetworkObject.h"
#include "Engine/NetDriver.h"
#include "GameFramework/Actor.h"

int32 UNetworkObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	auto* Owner = GetTypedOuter<AActor>();
	return Owner ? Owner->GetFunctionCallspace(Function, Stack) : FunctionCallspace::Local;
}

bool UNetworkObject::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
{
	if (AActor* MyOwner = GetTypedOuter<AActor>())
	{
		if (UNetDriver* NetDriver = MyOwner->GetNetDriver())
		{
			NetDriver->ProcessRemoteFunction(MyOwner, Function, Parameters, OutParms, Stack, this);
			return true;
		}
	}

	return false;
}
