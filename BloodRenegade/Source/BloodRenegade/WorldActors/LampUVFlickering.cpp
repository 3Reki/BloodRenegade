// Fill out your copyright notice in the Description page of Project Settings.


#include "LampUVFlickering.h"

void ALampUVFlickering::BeginPlay()
{
	Super::BeginPlay();
	
	FOnTimelineFloat FlickerCallBackF;
	TimelineIntensityFlicker.SetLooping(true);
	FlickerCallBackF.BindUFunction(this, FName("ChangeIntensity"));
	TimelineIntensityFlicker.AddInterpFloat(CurveFlickering.Get(), FlickerCallBackF);
	TimelineIntensityFlicker.PlayFromStart();
}


void ALampUVFlickering::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TimelineIntensityFlicker.TickTimeline(DeltaSeconds);
}

void ALampUVFlickering::ChangeIntensity(float f)
{
	LightSource->SetIntensity(f*100);
	if(f<=0.05f)
	{
		MeshCollider->SetGenerateOverlapEvents(false);
	}else
	{
		MeshCollider->SetGenerateOverlapEvents(true);
	}
}

