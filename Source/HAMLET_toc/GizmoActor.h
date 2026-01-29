// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GizmoActor.generated.h"

UCLASS()
class HAMLET_TOC_API AGizmoActor : public AActor
{
	GENERATED_BODY()

public:
	AGizmoActor();

private:
	UPROPERTY(VisibleAnywhere, Category = "Gizmo")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, Category = "Gizmo")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
};
