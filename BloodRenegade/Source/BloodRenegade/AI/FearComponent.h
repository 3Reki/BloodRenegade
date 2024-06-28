// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FearComponent.generated.h"

class USoldierDataAsset;
class UFearSubsystem;
class UMaterial;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOODRENEGADE_API UFearComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	USoldierDataAsset* SoldierDA;
	
	UFearComponent();

	UFUNCTION(BlueprintCallable)
	void AddFear(float Fear);

	UFUNCTION(BlueprintCallable)
	void MaxFear();

	UFUNCTION(BlueprintCallable)
	float GetEndFearTimer();

	UFUNCTION(BlueprintCallable)
	bool GetIsStressed();

	UFUNCTION(BlueprintCallable)
	float GetStressDetectionMultiplier();

	UFUNCTION(BlueprintCallable)
	void ChangeDefaultMaterial();

	UFUNCTION(BlueprintCallable)
	void ChangeFearMaterial();

protected:
	UFearSubsystem* FearSubsystem;

	float FearScore = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* DefaultMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* FearMaterial;

	void BeginPlay();


		
};
