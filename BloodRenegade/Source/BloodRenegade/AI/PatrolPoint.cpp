// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolPoint.h"

#include "Soldier.h"

#if WITH_EDITORONLY_DATA
void APatrolPoint::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MasterSoldier.IsValid())
	{
		MasterSoldier->RerunConstructionScripts();
	}
}
#endif

// Sets default values
APatrolPoint::APatrolPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	this->Location = this->GetActorLocation();
	this->Rotation = this->GetActorRotation();
	this->WaitTime = 0;

}

APatrolPoint::APatrolPoint(const FVector& Location, const FRotator& Rotation, const float WaitTime)
{
	this->Location = Location;
	this->Rotation = Rotation;
	this->WaitTime = WaitTime;
}

void APatrolPoint::BeginPlay() 
{
	this->Location = this->GetActorLocation();
	this->Rotation = this->GetActorRotation();
}

FVector APatrolPoint::GetLocation() const
{
	return Location;
}

FRotator APatrolPoint::GetRotation() const
{
	return Rotation;
}

float APatrolPoint::GetWaitTime() const
{
	return WaitTime;
}



