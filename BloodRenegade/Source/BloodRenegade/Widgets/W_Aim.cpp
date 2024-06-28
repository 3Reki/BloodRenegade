// Fill out your copyright notice in the Description page of Project Settings.


#include "W_Aim.h"

#include "Components/ProgressBar.h"

void UW_Aim::TargetInRange(bool b)
{
	if(b)
	{
		if(IsActive) return;
		IsActive = true;
		PlayAnimation(TargetClose.Get());
		return;
	}

	if(!IsActive) return;
	IsActive = false;
	PlayAnimationReverse(TargetClose.Get());
}

void UW_Aim::SetPercentCrossAir(float f)
{
	CrossAir->SetPercent(f);
}
