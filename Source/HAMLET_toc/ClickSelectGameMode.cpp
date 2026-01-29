// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClickSelectGameMode.h"

#include "ClickSelectPlayerController.h"

AClickSelectGameMode::AClickSelectGameMode()
{
	PlayerControllerClass = AClickSelectPlayerController::StaticClass();
}
