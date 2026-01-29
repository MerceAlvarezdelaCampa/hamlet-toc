// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ClickSelectPlayerController.generated.h"

UCLASS()
class HAMLET_TOC_API AClickSelectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY()
	TObjectPtr<AActor> SelectedActor;

	void HandleSelectPressed();
	void UpdateSelection(AActor* NewSelection);
	void SetActorSelected(AActor* Actor, bool bSelected) const;
};
