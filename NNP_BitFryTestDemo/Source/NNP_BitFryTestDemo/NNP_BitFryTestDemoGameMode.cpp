// Copyright Epic Games, Inc. All Rights Reserved.

#include "NNP_BitFryTestDemoGameMode.h"
#include "NNP_BitFryTestDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANNP_BitFryTestDemoGameMode::ANNP_BitFryTestDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
