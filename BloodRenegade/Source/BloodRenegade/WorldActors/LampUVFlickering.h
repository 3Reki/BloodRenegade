// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LampUV.h"
#include "Components/TimelineComponent.h"
#include "LampUVFlickering.generated.h"

/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API ALampUVFlickering : public ALampUV
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
protected:
	
	UFUNCTION()
	void ChangeIntensity(float f);

	UPROPERTY()
	FTimeline TimelineIntensityFlicker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CurveFlicker)
	TSoftObjectPtr<UCurveFloat> CurveFlickering;
};
