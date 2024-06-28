// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VibationDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UVibationDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,  Category = Damage)
	UForceFeedbackEffect* TakeItEffect;

	UPROPERTY(EditAnywhere,  Category = Spell)
	UForceFeedbackEffect* StartSpell;

	UPROPERTY(EditAnywhere,  Category = Jump)
	UForceFeedbackEffect* LoudLand;

	UPROPERTY(EditAnywhere, Category = Bite)
	UForceFeedbackEffect* BiteEffect;

	UPROPERTY(EditAnywhere, Category = Detection)
	UForceFeedbackEffect* VisionDetection;

	UPROPERTY(EditAnywhere, Category = Detection)
	UForceFeedbackEffect* HearingDetection;

	UPROPERTY(EditAnywhere, Category = Detection)
	UForceFeedbackEffect* SoldierAlerted;

	UPROPERTY(EditAnywhere, Category = Detection)
	UForceFeedbackEffect* InLight;
};
