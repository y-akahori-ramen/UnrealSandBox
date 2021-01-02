// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealSandBoxGameMode.h"
#include "UnrealSandBoxCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUnrealSandBoxGameMode::AUnrealSandBoxGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
