// Fill out your copyright notice in the Description page of Project Settings.


#include "AriasMechanisme.h"

#include "BloodRenegade/Controller/BloodCharacter.h"
#include "BloodRenegade/WorldActors/AriaObjectif.h"
#include "Components/SphereComponent.h"
// Sets default values
AAriasMechanisme::AAriasMechanisme()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AAriasMechanisme::StartActiveMechanisme()
{
	if(!Aria) return;
	
	Aria->NextObjectif();
}
