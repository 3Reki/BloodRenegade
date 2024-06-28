// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Plugins/Wwise/Source/AkAudio/Classes/AkGameplayStatics.h"
#include "Components/ActorComponent.h"
#include "TriggerEventType.h"
#include "BehaviorStates.h"
#include "../DialogueSystem/Voices.h"
#include "SoldierDialogueComponent.generated.h"

class UAkComponent;
class UAkAudioEvent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOODRENEGADE_API USoldierDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	USoldierDialogueComponent();

	UPROPERTY(EditAnywhere)
	UVoices SoldierVoice;

	int EventID;

	void PlaySound();
	void StopSound();

	void FindInterlocutors();

	void PlayDialogue(BehaviorStates From, BehaviorStates To);

	void PlayDialogue(TriggerEventType Trigger);

protected:
	UPROPERTY(EditAnywhere)
	float InterlocutorsDistance;

	TArray<USoldierDialogueComponent*> SDCInRange;

	// Called when the game starts
	virtual void BeginPlay() override;
};
