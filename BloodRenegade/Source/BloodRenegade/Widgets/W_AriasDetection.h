// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_AriasDetection.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UW_AriasDetection : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetPercent(float f);
private:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* AriaDetectionGauge;
};
