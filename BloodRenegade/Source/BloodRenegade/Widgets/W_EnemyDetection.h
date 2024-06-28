// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "W_EnemyDetection.generated.h"

/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UW_EnemyDetection : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetDetectionPercentage(float f);
	UFUNCTION(BlueprintCallable)
	void SetDetectBySight(bool b);
	UFUNCTION(BlueprintCallable)
	void SetDetectByHearing(bool b);
	UFUNCTION(BlueprintCallable)
	void SetIsShooting(bool b);
	UFUNCTION(BlueprintCallable)
	void SetIsShielded(bool b);
	UFUNCTION(BlueprintCallable)
	void SetHasSeenEnemyRecently(bool b);
	UFUNCTION(BlueprintCallable)
	void SetEnemyVisible(bool b);
	UFUNCTION(BlueprintCallable)
	void SetArrowVisible(bool b);
	UFUNCTION(BlueprintCallable)
	void SetArrowAngle(float Angle);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateColor NeutralColor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateColor SuspiciousColor;

	UPROPERTY(BlueprintReadWrite)
	float DetectionPercentage = 0;
	UPROPERTY(BlueprintReadWrite)
	bool DetectBySight = false;
	UPROPERTY(BlueprintReadWrite)
	bool DetectByHearing = false;
	UPROPERTY(BlueprintReadWrite)
	bool IsShooting = false;
	UPROPERTY(BlueprintReadOnly)
	bool IsShielded = false;
	UPROPERTY(BlueprintReadWrite)
	bool HasSeenEnemyRecently = false;
	UPROPERTY(BlueprintReadWrite)
	bool EnemyVisible = false;
	UPROPERTY(BlueprintReadWrite)
	bool ArrowVisible = false;
	UPROPERTY(BlueprintReadWrite)
	float ArrowAngle;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UProgressBar> NeutralPB;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UImage> Arrow;
};
