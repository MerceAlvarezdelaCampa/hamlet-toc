// Copyright Epic Games, Inc. All Rights Reserved.

#include "RuntimeMoveGizmoActor.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/CollisionProfile.h"
#include "UObject/ConstructorHelpers.h"

ARuntimeMoveGizmoActor::ARuntimeMoveGizmoActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GizmoMesh"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetMobility(EComponentMobility::Movable);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Game/Axis_Guide.Axis_Guide"));
	if (!MeshFinder.Succeeded())
	{
		static ConstructorHelpers::FObjectFinder<UStaticMesh> FallbackMeshFinder(TEXT("/Engine/BasicShapes/Arrow.Arrow"));
		if (FallbackMeshFinder.Succeeded())
		{
			MeshComponent->SetStaticMesh(FallbackMeshFinder.Object);
			MeshComponent->SetWorldScale3D(FVector(0.5f));
		}
	}
	else
	{
		MeshComponent->SetStaticMesh(MeshFinder.Object);
	}

	if (MeshComponent->GetStaticMesh())
	{
		MeshComponent->SetWorldScale3D(FVector(0.5f));
	}
}
