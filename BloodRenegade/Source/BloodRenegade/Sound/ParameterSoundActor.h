// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AkComponent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "ParameterSoundActor.generated.h"

UCLASS()
class BLOODRENEGADE_API AParameterSoundActor : public AActor
{
	GENERATED_BODY()
	
protected:
	AParameterSoundActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category="Variables", meta = (AllowPrivateAccess="true"))
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, Category="Variables", meta = (AllowPrivateAccess="true"))
	UAkComponent* AkComponent;	
	
	UPROPERTY(EditAnywhere, Category="Variables", meta = (AllowPrivateAccess="true"))
	UTextRenderComponent* Text;

	UPROPERTY(EditAnywhere, Category="Variables", meta = (AllowPrivateAccess="true", ClampMin = "1"))
	float MinToMaxDuration = 3.0f;

	// UPROPERTY(EditAnywhere, Category="Variables", meta = (AllowPrivateAccess="true"))
	// TEnumAsByte<EPlayType> PlayType = EPlayType::BeginPlay;
	
	UAkAudioEvent* AudioEvent;
	float PitchValue;	
	
	void Play() const;
};