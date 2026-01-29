// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClickSelectPlayerController.h"

#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/PrimitiveComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

void AClickSelectPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	if (IConsoleVariable* CustomDepthVar = IConsoleManager::Get().FindConsoleVariable(TEXT("r.CustomDepth")))
	{
		CustomDepthVar->Set(3, ECVF_SetByCode);
	}

	ConfigureOutlinePostProcess();
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
		PrimitiveComponent->SetCustomDepthStencilValue(bSelected ? SelectionStencilValue : 0);
	}
}

void AClickSelectPlayerController::ConfigureOutlinePostProcess()
{
	if (!SelectionOutlineMaterial || !PlayerCameraManager)
	{
		return;
	}

	SelectionOutlineMID = UMaterialInstanceDynamic::Create(SelectionOutlineMaterial, this);
	if (!SelectionOutlineMID)
	{
		return;
	}

	SelectionOutlineMID->SetVectorParameterValue(TEXT("OutlineColor"), SelectionOutlineColor);
	PlayerCameraManager->ViewTarget.POV.PostProcessSettings.AddBlendable(SelectionOutlineMID, 1.0f);
}
