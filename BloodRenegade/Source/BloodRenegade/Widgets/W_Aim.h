// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_Aim.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UW_Aim : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UProgressBar> CrossAir;

	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim), Transient)
	TWeakObjectPtr<UWidgetAnimation> TargetClose;

	bool IsActive;
	
public:
	void TargetInRange(bool b);
	void SetPercentCrossAir(float f);
};
