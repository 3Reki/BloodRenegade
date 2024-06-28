// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_HookViewport.generated.h"

class UProgressBar;
/**
 * 
 */
class UImage;

UCLASS()
class BLOODRENEGADE_API UW_HookViewport : public UUserWidget
{
	GENERATED_BODY()

public:
	void MovePointer(FVector2D NewPosition, float dist);
	void SetCursorPercent(float f);
	void CantHook();
	void SetActiveHook(bool b);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UTexture2D*> HookAssetList;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UImage> HookPointerBorder;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UProgressBar> HookPointerPercent;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UImage> HookCross;

	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim), Transient)
	TWeakObjectPtr<UWidgetAnimation> HookCrossAnim;
};
