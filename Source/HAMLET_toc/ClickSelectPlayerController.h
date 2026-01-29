// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ClickSelectPlayerController.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;
class AGizmoActor;

UCLASS()
class HAMLET_TOC_API AClickSelectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Selection|Outline", meta = (ClampMin = "1", ClampMax = "255"))
	int32 SelectionStencilValue = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Selection|Outline")
	FLinearColor SelectionOutlineColor = FLinearColor(0.0f, 0.5f, 0.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Selection|Outline")
	TObjectPtr<UMaterialInterface> SelectionOutlineMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> SelectionOutlineMID;

	UPROPERTY()
	TObjectPtr<AActor> SelectedActor;

	UPROPERTY(EditDefaultsOnly, Category = "Selection|Gizmo")
	TSubclassOf<AGizmoActor> GizmoClass;

	UPROPERTY(Transient)
	TObjectPtr<AGizmoActor> GizmoActor;

	bool bIsDraggingGizmo = false;
	FPlane DragPlane = FPlane(FVector::ZeroVector, FVector::UpVector);
	FVector DragOffset = FVector::ZeroVector;

	void HandleSelectPressed();
	void HandleTranslatePressed();
	void HandleGizmoDragPressed();
	void HandleGizmoDragReleased();
	void UpdateGizmoDrag(float DeltaSeconds);
	void SpawnOrMoveGizmo();
	void UpdateSelection(AActor* NewSelection);
	void SetActorSelected(AActor* Actor, bool bSelected) const;
	void ConfigureOutlinePostProcess();
};
