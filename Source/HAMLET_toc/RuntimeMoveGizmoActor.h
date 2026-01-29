// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuntimeMoveGizmoActor.generated.h"

UCLASS()
class HAMLET_TOC_API ARuntimeMoveGizmoActor : public AActor
{
	GENERATED_BODY()

public:
	ARuntimeMoveGizmoActor();

private:
	UPROPERTY(VisibleAnywhere, Category = "Gizmo")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category = "Gizmo")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
};
