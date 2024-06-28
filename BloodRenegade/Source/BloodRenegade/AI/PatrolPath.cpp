// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolPath.h"
#include "Components/ArrowComponent.h"
#include "../DataAssets/PatrolDataAsset.h"
#include "Components/TextRenderComponent.h"
#include "Soldier.h"
#include <Kismet/KismetTextLibrary.h>

// Sets default values
APatrolPath::APatrolPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	SplineComponent->SetupAttachment(RootComponent);
}

void APatrolPath::AddSoldier(ASoldier* Soldier)
{
	Soldiers.AddUnique(Soldier);
	HasReachedPoint.Add(false);
}

void APatrolPath::RemoveSoldier(ASoldier* Soldier)
{
	int idx = Soldiers.Find(Soldier);
	check(idx > -1);
	Soldiers.RemoveAt(idx);
	HasReachedPoint.RemoveAt(idx);
}

bool APatrolPath::ContainsSoldier(ASoldier* Soldier)
{
	return Soldiers.Contains(Soldier);
}

void APatrolPath::SoldierReachedPoint(ASoldier* Soldier)
{
	int idx = Soldiers.Find(Soldier);
	if (idx < 0 || idx >= HasReachedPoint.Num()) return;
	HasReachedPoint[idx] = true;
	CheckAllSoldiersReachedPoint();
}

FVector APatrolPath::GetPatrolPointLocation(ASoldier* Soldier)
{
	int SoldierIdx = Soldiers.Find(Soldier);
	if (SoldierIdx < 0 || Soldiers.Num() > DAPatrol->PatrolFormation.Num() || SoldierIdx >= DAPatrol->PatrolFormation[Soldiers.Num() - 1].Offsets.Num())
	{
		return FVector(0, 0, 0);
	}
	FVector PPLocation = SplineComponent->GetLocationAtDistanceAlongSpline(SplineComponent->GetSplineLength() / 100 * PatrolPoints[CurrentPatrolPointIdx].PPDistancePercentage, ESplineCoordinateSpace::World);
	if(UsePathRotation) PPLocation += SplineComponent->GetRotationAtDistanceAlongSpline(SplineComponent->GetSplineLength() / 100 * PatrolPoints[CurrentPatrolPointIdx].PPDistancePercentage, ESplineCoordinateSpace::World).RotateVector(DAPatrol->PatrolFormation[Soldiers.Num() - 1].Offsets[SoldierIdx]);
	else PPLocation += PatrolPoints[CurrentPatrolPointIdx].PPRotator.RotateVector(DAPatrol->PatrolFormation[Soldiers.Num() - 1].Offsets[SoldierIdx]);
	return PPLocation;
}

FRotator APatrolPath::GetPatrolPointRotation()
{
	if (UsePathRotation) return SplineComponent->GetRotationAtDistanceAlongSpline(SplineComponent->GetSplineLength() / 100 * PatrolPoints[CurrentPatrolPointIdx].PPDistancePercentage, ESplineCoordinateSpace::World);
	else return PatrolPoints[CurrentPatrolPointIdx].PPRotator;
}

int APatrolPath::GetNextPointIdx()
{
	
	if (!IsPathLooping) 
	{
		if (CurrentPatrolPointIdx + PathDirection < 0 || CurrentPatrolPointIdx + PathDirection > PatrolPoints.Num() - 1) PathDirection *= -1;
	}
	CurrentPatrolPointIdx = (CurrentPatrolPointIdx + PathDirection) % PatrolPoints.Num();
	
	return CurrentPatrolPointIdx;
}

void APatrolPath::SetCurrentPatrolPointIdx(int Idx)
{
	CurrentPatrolPointIdx = Idx;
}

FSplinePatrolPoint APatrolPath::GetPatrolPoint()
{
	return PatrolPoints[CurrentPatrolPointIdx];
}

int APatrolPath::GetNBPatrolPoints()
{
	return PatrolPoints.Num();
}

void APatrolPath::BeginPlay()
{
	Super::BeginPlay();
	//SplineComponent->SetClosedLoop(IsPathLooping);
	//for(int i = 0; i<Soldiers.Num())
}

void APatrolPath::CheckAllSoldiersReachedPoint() 
{
	bool AllSoldiersReachedPoint = true;
	for (bool b : HasReachedPoint) 
	{
		if (!b) AllSoldiersReachedPoint = false;
	}

	if (AllSoldiersReachedPoint) 
	{
		for (int i = 0; i < HasReachedPoint.Num(); i++) 
		{
			HasReachedPoint[i] = false;
		}

		for (ASoldier* Soldier : Soldiers) 
		{

			Soldier->WaitPatrolPoint(GetPatrolPoint().WaitTime);
		}
		GetNextPointIdx();
	}
}

#if WITH_EDITORONLY_DATA
void APatrolPath::OnConstruction(const FTransform& Transform) 
{
	Super::OnConstruction(Transform);

	for (int i = 0; i < PatrolPoints.Num(); i++) 
	{
		UArrowComponent* ArrowComponent = (UArrowComponent*)AddComponentByClass(UArrowComponent::StaticClass(), false, FTransform::Identity, false);
		ArrowComponent->SetWorldLocation(SplineComponent->GetLocationAtDistanceAlongSpline(SplineComponent->GetSplineLength() / 100 * PatrolPoints[i].PPDistancePercentage, ESplineCoordinateSpace::World));
		if(UsePathRotation) ArrowComponent->SetWorldRotation(SplineComponent->GetRotationAtDistanceAlongSpline(SplineComponent->GetSplineLength() / 100 * PatrolPoints[i].PPDistancePercentage, ESplineCoordinateSpace::World));
		else ArrowComponent->SetWorldRotation(PatrolPoints[i].PPRotator);
		ArrowComponent->SetArrowColor(FLinearColor::Green);
		ArrowComponent->ArrowLength = ConstructionArrowLength;
		ArrowComponent->ArrowSize = ConstructionArrowSize;

		UTextRenderComponent* TextRenderComponent = (UTextRenderComponent*)
			AddComponentByClass(UTextRenderComponent::StaticClass(), true, FTransform::Identity, false);

		TextRenderComponent->SetWorldLocation(SplineComponent->GetLocationAtDistanceAlongSpline(SplineComponent->GetSplineLength() / 100 * PatrolPoints[i].PPDistancePercentage, ESplineCoordinateSpace::World));
		TextRenderComponent->SetText(UKismetTextLibrary::Conv_IntToText(i));
		TextRenderComponent->SetHiddenInGame(true);
	}
}
#endif

