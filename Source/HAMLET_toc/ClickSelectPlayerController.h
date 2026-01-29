// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ClickSelectPlayerController.generated.h"

class UMaterialInstanceDynamic;
class UMaterialInterface;

UCLASS()
class HAMLET_TOC_API AClickSelectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Selection|Outline")
	void SetSelectionOutlineColor(const FLinearColor& NewColor);

private:
	UPROPERTY(EditDefaultsOnly, Category = "Selection|Outline", meta = (ClampMin = "1", ClampMax = "255"))
	int32 SelectionStencilValue = 1;

	UPROPERTY(EditAnywhere, Category = "Selection|Outline")
	FLinearColor SelectionOutlineColor = FLinearColor(1.0f, 0.5f, 0.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Selection|Outline")
	TObjectPtr<UMaterialInterface> SelectionOutlineMaterial;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> SelectionOutlineMID;

	UPROPERTY()
	TObjectPtr<AActor> SelectedActor;

	void HandleSelectPressed();
	void UpdateSelection(AActor* NewSelection);
	void SetActorSelected(AActor* Actor, bool bSelected) const;
	void ConfigureOutlinePostProcess();
};
