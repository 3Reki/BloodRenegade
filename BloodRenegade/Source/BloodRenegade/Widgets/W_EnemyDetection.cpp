// Fill out your copyright notice in the Description page of Project Settings.


#include "W_EnemyDetection.h"

void UW_EnemyDetection::SetDetectionPercentage(float f)
{
	DetectionPercentage = f;

	if (DetectionPercentage < 200)
	{
		NeutralPB.Get()->WidgetStyle.BackgroundImage.TintColor = FSlateColor(FLinearColor(0, 0, 0, 0));
		NeutralPB.Get()->WidgetStyle.FillImage.TintColor = NeutralColor;
	}
	else 
	{
		NeutralPB.Get()->WidgetStyle.BackgroundImage.TintColor = SuspiciousColor;
		NeutralPB.Get()->WidgetStyle.FillImage.TintColor = SuspiciousColor;
	}
}

void UW_EnemyDetection::SetDetectBySight(bool b)
{
	DetectBySight = b;
}

void UW_EnemyDetection::SetDetectByHearing(bool b)
{
	DetectByHearing = b;
}

void UW_EnemyDetection::SetIsShooting(bool b)
{
	IsShooting = b;
}

void UW_EnemyDetection::SetIsShielded(bool b) 
{
	IsShielded = b;
}

void UW_EnemyDetection::SetHasSeenEnemyRecently(bool b) 
{
	HasSeenEnemyRecently = b;
}

void UW_EnemyDetection::SetEnemyVisible(bool b) 
{
	EnemyVisible = b;
}

void UW_EnemyDetection::SetArrowVisible(bool b)
{
	ArrowVisible = b;
}

void UW_EnemyDetection::SetArrowAngle(float Angle)
{
	ArrowAngle = Angle;
	Arrow.Get()->SetRenderTransformAngle(Angle);
}
