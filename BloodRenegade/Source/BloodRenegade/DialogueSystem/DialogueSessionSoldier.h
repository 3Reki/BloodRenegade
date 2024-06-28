// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueSession.h"
#include "../AI/BehaviorStates.h"
#include "../AI/TriggerEventType.h"
#include "DialogueSessionSoldier.generated.h"

/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UDialogueSessionSoldier : public UDialogueSession
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	BehaviorStates StateFrom;

	UPROPERTY(EditAnywhere)
	BehaviorStates StateTo;

	UPROPERTY(EditAnywhere)
	TriggerEventType Trigger;

	UPROPERTY(EditAnywhere)
	bool IsStressed;
};
