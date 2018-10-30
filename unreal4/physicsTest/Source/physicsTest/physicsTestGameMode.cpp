// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "physicsTestGameMode.h"
#include "physicsTestHUD.h"
#include "physicsTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AphysicsTestGameMode::AphysicsTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AphysicsTestHUD::StaticClass();
}
