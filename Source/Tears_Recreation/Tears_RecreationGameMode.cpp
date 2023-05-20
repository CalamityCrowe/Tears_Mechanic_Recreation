// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tears_RecreationGameMode.h"
#include "Tears_RecreationCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATears_RecreationGameMode::ATears_RecreationGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
