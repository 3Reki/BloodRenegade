// Fill out your copyright notice in the Description page of Project Settings.


#include "FearComponent.h"
#include "../Subsystems/GameInstance/FearSubsystem.h"
#include "../DataAssets/SoldierDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Soldier.h"

void UFearComponent::BeginPlay() 
{
	Super::BeginPlay();

	TWeakObjectPtr<UGameInstance> GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	FearSubsystem = GameInstance.Get()->GetSubsystem<UFearSubsystem>();

	FearScore = 0;
}

// Sets default values for this component's properties
UFearComponent::UFearComponent()
{

}

void UFearComponent::AddFear(float Fear)
{
	float DeltaFear = Fear;
	FearScore += Fear;

	if (FearScore > SoldierDA->SoldierMaxFear) 
	{
		DeltaFear -= (FearScore - SoldierDA->SoldierMaxFear);
		FearScore = SoldierDA->SoldierMaxFear;
	}

	FearSubsystem->OnFearChangedDelegate.ExecuteIfBound(DeltaFear);
}

void UFearComponent::MaxFear() 
{
	FearScore = SoldierDA->SoldierMaxFear;
}

float UFearComponent::GetEndFearTimer() 
{
	return SoldierDA->EndFearTimer;
}

bool UFearComponent::GetIsStressed() 
{
	return FearScore == SoldierDA->SoldierMaxFear;
}

float UFearComponent::GetStressDetectionMultiplier() 
{
	return SoldierDA->StressDetectionMultiplier;
}

void UFearComponent::ChangeDefaultMaterial() 
{
	ASoldier* Soldier = Cast<ASoldier>(GetOwner());
	TArray<UMeshComponent*> MeshComponents;
	Soldier->GetComponents<UMeshComponent>(MeshComponents);
	MeshComponents[0]->SetMaterial(0, DefaultMaterial);
}

void UFearComponent::ChangeFearMaterial() {
	ASoldier* Soldier = Cast<ASoldier>(GetOwner());
	TArray<UMeshComponent*> MeshComponents;
	Soldier->GetComponents<UMeshComponent>(MeshComponents);
	MeshComponents[0]->SetMaterial(0, FearMaterial);
}

