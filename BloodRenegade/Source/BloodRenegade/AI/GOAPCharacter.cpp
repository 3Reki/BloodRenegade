// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPCharacter.h"

AGOAPCharacter::AGOAPCharacter() 
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGOAPCharacter::BeginPlay() 
{
	Super::BeginPlay();
}

void AGOAPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGOAPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

}
