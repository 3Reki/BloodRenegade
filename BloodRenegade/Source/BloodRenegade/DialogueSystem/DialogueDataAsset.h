// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "../AI/BehaviorStates.h"
#include "../AI/TriggerEventType.h"
#include "DialogueDataAsset.generated.h"
/**
 * 
 */
class UDialogueSession;
class UDialogueSessionSoldier;

UCLASS(BlueprintType, Blueprintable)
class BLOODRENEGADE_API UDialogueDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	float SubtitleTimer = 4;

	UPROPERTY(EditAnywhere, Category = "Anything")
	TArray<UDialogueSession*> Dialogues;

	UPROPERTY(EditAnywhere, Category = "Soldier")
	TArray<UDialogueSessionSoldier*> SoldierDialogues;

	void ResetAllDialoguesActivation();

	TArray<UDialogueSession*> GetDialogues(int MinInterlocutors, int MaxInterlocutors);

	TArray<UDialogueSession*> GetDialoguesSoldier(int MinInterlocutors, int MaxInterlocutors, TriggerEventType Trigger);

	TArray<UDialogueSession*> GetDialoguesSoldier(int MinInterlocutors, int MaxInterlocutors, TriggerEventType Trigger, bool IsStressed);

	TArray<UDialogueSession*> GetDialoguesSoldier(int MinInterlocutors, int MaxInterlocutors, BehaviorStates From, BehaviorStates To);

	TArray<UDialogueSession*> GetDialoguesSoldier(int MinInterlocutors, int MaxInterlocutors, BehaviorStates From, BehaviorStates To, bool IsStressed);
};
