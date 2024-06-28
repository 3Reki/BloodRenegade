// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AriasDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UAriasDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AriaSentinel)
	float MaxGauge = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AriaSentinel)
	float TimingWaitForDecreaseGauge = 8;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AriaSentinel)
	float SearchingSpeed = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AriaSentinel)
	float TimingWaitForPatrol = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = AriaObjectiv)
	float SpeedMoveToNextObj = 5;

};
