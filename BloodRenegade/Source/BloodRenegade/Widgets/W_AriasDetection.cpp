// Fill out your copyright notice in the Description page of Project Settings.


#include "W_AriasDetection.h"

#include "Components/ProgressBar.h"

void UW_AriasDetection::SetPercent(float f)
{
	AriaDetectionGauge->SetPercent(f);
}
