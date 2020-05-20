// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "ProjectRGameMode.h"
#include "ProjectRCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectRGameMode::AProjectRGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
