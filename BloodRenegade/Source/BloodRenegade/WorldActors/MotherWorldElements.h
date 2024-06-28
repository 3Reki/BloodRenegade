// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MotherWorldElements.generated.h"

UCLASS()
class BLOODRENEGADE_API AMotherWorldElements : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMotherWorldElements();

	UFUNCTION(Blueprintable, BlueprintImplementableEvent, BlueprintCallable)
	void SwitchOn();

	UFUNCTION(Blueprintable, BlueprintImplementableEvent, BlueprintCallable)
	void SwitchOff();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
