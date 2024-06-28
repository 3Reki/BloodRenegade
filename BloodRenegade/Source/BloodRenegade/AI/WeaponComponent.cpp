// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "../Controller/BloodCharacter.h"
#include "Components/TimelineComponent.h"
#include "TimerManager.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Curves/CurveFloat.h"
#include "Perception/AISense_Hearing.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	CanFire = true;
}

void UWeaponComponent::_FinishFire()
{
	CanFire = true;
}

void UWeaponComponent::Fire(FVector direction, FVector startLocation)
{
	FVector endPoint = _FireTrace(direction, startLocation);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetOwner()->GetActorLocation(), 1, nullptr, SoldierDA->ShotSoundRange, FName(TEXT("Instant")));
}

FVector UWeaponComponent::_FireTrace(FVector& direction, FVector startLocation)
{
	FHitResult hit;
	FVector traceStart = startLocation;
	direction.Normalize();
	FVector traceEnd = traceStart + (direction * SoldierDA->FireRange);

	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(hit, traceStart, traceEnd, TraceChannelPorperty, queryParams);
	//DrawDebugLine(GetWorld(), traceStart, traceEnd, hit.bBlockingHit ? FColor::Blue : FColor::Red, false, 5, 0, 10);
	ABloodCharacter* bloodCharacter = Cast<ABloodCharacter>(hit.GetActor());
	if (bloodCharacter) 
	{
		bloodCharacter->TakeHit(SoldierDA->WeaponDamage);
	}

	return hit.ImpactPoint;

	
}

bool UWeaponComponent::GetCanFire()
{
	return CanFire;
}

void UWeaponComponent::SetMuzzlePoint(USceneComponent* SceneComponent)
{
	MuzzlePoint = SceneComponent;
}

