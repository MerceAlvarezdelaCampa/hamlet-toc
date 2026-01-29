// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClickSelectPlayerController.h"

#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Components/PrimitiveComponent.h"

void AClickSelectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void AClickSelectPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction("Select", IE_Pressed, this, &AClickSelectPlayerController::HandleSelectPressed);
	}
}

void AClickSelectPlayerController::HandleSelectPressed()
{
	FHitResult HitResult;
	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	const bool bHit = GetHitResultUnderCursorByChannel(TraceChannel, true, HitResult);
	AActor* HitActor = bHit ? HitResult.GetActor() : nullptr;

	UpdateSelection(HitActor);
}

void AClickSelectPlayerController::UpdateSelection(AActor* NewSelection)
{
	if (SelectedActor == NewSelection)
	{
		return;
	}

	if (SelectedActor)
	{
		SetActorSelected(SelectedActor, false);
	}

	SelectedActor = NewSelection;

	if (SelectedActor)
	{
		SetActorSelected(SelectedActor, true);
		UE_LOG(LogTemp, Log, TEXT("Selected actor: %s"), *SelectedActor->GetName());
	}
}

void AClickSelectPlayerController::SetActorSelected(AActor* Actor, bool bSelected) const
{
	if (!Actor)
	{
		return;
	}

	TInlineComponentArray<UPrimitiveComponent*> PrimitiveComponents;
	Actor->GetComponents(PrimitiveComponents);
	for (UPrimitiveComponent* PrimitiveComponent : PrimitiveComponents)
	{
		if (!PrimitiveComponent)
		{
			continue;
		}

		PrimitiveComponent->SetRenderCustomDepth(bSelected);
		if (bSelected)
		{
			PrimitiveComponent->SetCustomDepthStencilValue(1);
		}
	}
}
