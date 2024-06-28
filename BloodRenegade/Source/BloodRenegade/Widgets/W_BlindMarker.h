// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_BlindMarker.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UW_BlindMarker : public UUserWidget
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* TargetLock;
	
	void SetActiveTarget(bool b);
};
