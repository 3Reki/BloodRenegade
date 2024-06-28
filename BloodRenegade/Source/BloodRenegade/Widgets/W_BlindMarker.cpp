// Fill out your copyright notice in the Description page of Project Settings.


#include "W_BlindMarker.h"

void UW_BlindMarker::SetActiveTarget(bool b)
{
	if(b)
	{
		SetRenderOpacity(1);
		return;
	}
	
	SetRenderOpacity(0);
}
