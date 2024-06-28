// Fill out your copyright notice in the Description page of Project Settings.


#include "SearchingLampUV.h"
#include "BloodRenegade/WorldActors/LampUVDataAsset.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"


ASearchingLampUV::ASearchingLampUV()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	SplineComponent->SetupAttachment(RootComponent);
}

void ASearchingLampUV::BeingBlind_Implementation(float f)
{
	IIBlindable::BeingBlind_Implementation(f);
	LocalSwitchOff();
	GetWorldTimerManager().SetTimer(SwitchOffTimer, this, &ASearchingLampUV::LocalSwitchOn, DataAsset->BlindTimer, false);
}

void ASearchingLampUV::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	for(int j = 0; j<SplineComponent->GetNumberOfSplinePoints(); j++)
	{
		if(j>=PointsTiming.Num())
		{
			PointsTiming.Add(0);
		}
	}

	for(int j = PointsTiming.Num()-1; j>=0; j--)
	{
		if(j>=SplineComponent->GetNumberOfSplinePoints())
		{
			PointsTiming.RemoveAt(j);
		}
	}
}

void ASearchingLampUV::BeginPlay()
{
	Super::BeginPlay();
	NextPoint = 1;
	FRotator rotation = UKismetMathLibrary::FindLookAtRotation(LightSource->GetComponentLocation(), SplineComponent->GetLocationAtTime(0, ESplineCoordinateSpace::World));
	LightSource->SetWorldRotation(rotation);
	
	RevolutionCurve = NewObject<UCurveFloat>();
	RevolutionCurve->FloatCurve.AddKey(0,0);
	RevolutionCurve->FloatCurve.AddKey(DataAsset->SpeedSearching,NextPoint/(SplineComponent->GetNumberOfSplinePoints()-1));

	FOnTimelineFloat RevolutionCallBack;
	FOnTimelineEventStatic RevolutionCallBackStatic;
	
	RevolutionCallBack.BindUFunction(this, FName("RotateTowardPoint"));
	TimelineForRevolution.AddInterpFloat(RevolutionCurve, RevolutionCallBack);
	
	RevolutionCallBackStatic.BindUFunction(this, FName("Next"));
	TimelineForRevolution.SetTimelineFinishedFunc(RevolutionCallBackStatic);

	GetWorld()->GetTimerManager().ClearTimer(WaitForNextPointTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(WaitForNextPointTimerHandle, this, &ASearchingLampUV::StartTimelineCheck, PointsTiming[0], false);
}

void ASearchingLampUV::LocalSwitchOff()
{
	SwitchOff();
	TimelineForRevolution.Stop();
}

void ASearchingLampUV::LocalSwitchOn()
{
	SwitchOn();
	TimelineForRevolution.Play();
}

void ASearchingLampUV::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TimelineForRevolution.TickTimeline(DeltaSeconds);
}

void ASearchingLampUV::RotateTowardPoint(float Value)
{
	FRotator rotation = UKismetMathLibrary::FindLookAtRotation(LightSource->GetComponentLocation(), SplineComponent->GetLocationAtTime(Value, ESplineCoordinateSpace::World));
	LightSource->SetWorldRotation(rotation);
}

void ASearchingLampUV::Next()
{
	GetWorld()->GetTimerManager().ClearTimer(WaitForNextPointTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(WaitForNextPointTimerHandle, this, &ASearchingLampUV::StartTimelineCheck, PointsTiming[NextPoint], false);

	RevolutionCurve->FloatCurve.UpdateOrAddKey(0,NextPoint/(SplineComponent->GetNumberOfSplinePoints()-1));
	
	if(NextPoint == SplineComponent->GetNumberOfSplinePoints()-1 && !Reverse)
	{
		Reverse = true;
	}else if(NextPoint == 0 && Reverse)
	{
		Reverse = false;
	}
	
	if(Reverse)
	{
		NextPoint--;
	}else
	{
		NextPoint++;
	}
	
	RevolutionCurve->FloatCurve.UpdateOrAddKey(DataAsset->SpeedSearching,NextPoint/(SplineComponent->GetNumberOfSplinePoints()-1));
}

void ASearchingLampUV::StartTimelineCheck()
{
	TimelineForRevolution.PlayFromStart();
}
