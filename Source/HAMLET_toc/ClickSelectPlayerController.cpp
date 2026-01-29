// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClickSelectPlayerController.h"

#include "RuntimeMoveGizmoActor.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "HAL/IConsoleManager.h"
#include "InputCoreTypes.h"
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
		InputComponent->BindKey(EKeys::W, IE_Pressed, this, &AClickSelectPlayerController::HandleTranslatePressed);
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AClickSelectPlayerController::HandleGizmoDragPressed);
		InputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AClickSelectPlayerController::HandleGizmoDragReleased);
	}
}

void AClickSelectPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateGizmoDrag(DeltaSeconds);
}

void AClickSelectPlayerController::HandleSelectPressed()
{
	FHitResult HitResult;
	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	const bool bHit = GetHitResultUnderCursorByChannel(TraceChannel, true, HitResult);
	AActor* HitActor = bHit ? HitResult.GetActor() : nullptr;

	if (HitActor && GizmoActor && HitActor == GizmoActor)
	{
		return;
	}

	UpdateSelection(HitActor);
}

void AClickSelectPlayerController::HandleTranslatePressed()
{
	if (!SelectedActor)
	{
		return;
	}

	if (GizmoActor)
	{
		const bool bIsHidden = GizmoActor->IsActorHiddenInGame();
		GizmoActor->SetActorHiddenInGame(!bIsHidden);
		GizmoActor->SetActorEnableCollision(bIsHidden);
		bIsDraggingGizmo = false;
		if (bIsHidden)
		{
			GizmoActor->SetActorLocation(SelectedActor->GetActorLocation());
		}
		return;
	}

	SpawnOrMoveGizmo();
}

void AClickSelectPlayerController::HandleGizmoDragPressed()
{
	if (!GizmoActor)
	{
		return;
	}

	if (GizmoActor->IsActorHiddenInGame())
	{
		return;
	}

	FHitResult HitResult;
	const ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);
	const bool bHit = GetHitResultUnderCursorByChannel(TraceChannel, true, HitResult);
	if (!bHit || HitResult.GetActor() != GizmoActor)
	{
		return;
	}

	bIsDraggingGizmo = true;
	const FVector GizmoLocation = GizmoActor->GetActorLocation();
	DragPlane = FPlane(GizmoLocation, FVector::UpVector);
	DragOffset = GizmoLocation - HitResult.Location;
}

void AClickSelectPlayerController::HandleGizmoDragReleased()
{
	bIsDraggingGizmo = false;
}

void AClickSelectPlayerController::UpdateGizmoDrag(float DeltaSeconds)
{
	if (!bIsDraggingGizmo || !GizmoActor)
	{
		return;
	}

	if (GizmoActor->IsActorHiddenInGame())
	{
		return;
	}

	FVector WorldOrigin;
	FVector WorldDirection;
	if (!DeprojectMousePositionToWorld(WorldOrigin, WorldDirection))
	{
		return;
	}

	const FVector RayEnd = WorldOrigin + WorldDirection * 100000.0f;
	const FVector Intersection = FMath::LinePlaneIntersection(WorldOrigin, RayEnd, DragPlane);
	const FVector NewLocation = Intersection + DragOffset;

	GizmoActor->SetActorLocation(NewLocation);
	if (SelectedActor)
	{
		SelectedActor->SetActorLocation(NewLocation);
	}
}

void AClickSelectPlayerController::SpawnOrMoveGizmo()
{
	if (!GizmoClass)
	{
		GizmoClass = ARuntimeMoveGizmoActor::StaticClass();
	}

	const FVector GizmoSpawnLocation = SelectedActor ? SelectedActor->GetActorLocation() : FVector::ZeroVector;

	if (!GizmoActor)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		GizmoActor = GetWorld()->SpawnActor<ARuntimeMoveGizmoActor>(GizmoClass, GizmoSpawnLocation, FRotator::ZeroRotator, SpawnParams);
	}
	else
	{
		GizmoActor->SetActorLocation(GizmoSpawnLocation);
	}
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
		if (GizmoActor)
		{
			GizmoActor->SetActorLocation(SelectedActor->GetActorLocation());
		}
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
	if (!PlayerCameraManager)
	{
		return;
	}

	if (!SelectionOutlineMaterial)
	{
		SelectionOutlineMaterial = LoadObject<UMaterialInterface>(
			nullptr,
			TEXT("/Engine/EditorMaterials/SelectionOutlineMaterial.SelectionOutlineMaterial"));
	}

	if (!SelectionOutlineMaterial)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectionOutlineMaterial is not set; assign a post-process outline material to enable selection highlighting."));
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
