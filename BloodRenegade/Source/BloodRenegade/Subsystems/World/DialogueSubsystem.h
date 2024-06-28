// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "../../AI/BehaviorStates.h"
#include "../../AI/TriggerEventType.h"
#include "../../DialogueSystem/Voices.h"
#include "AkAudioEvent.h"
#include "DialogueSubsystem.generated.h"


class UDialogueSession;
class UDialogueDataAsset;
class UAkComponent;
class USoldierDialogueComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class BLOODRENEGADE_API UDialogueSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	void PlayDialogue(TArray<USoldierDialogueComponent*> SourceList);

	void PlaySoldierDialogue(TArray<USoldierDialogueComponent*> SourceList, BehaviorStates From, BehaviorStates To);

	void PlaySoldierDialogue(TArray<USoldierDialogueComponent*> SourceList, BehaviorStates From, BehaviorStates To, bool IsStressed);

	void PlaySoldierDialogue(TArray<USoldierDialogueComponent*> SourceList, TriggerEventType Trigger);

	void PlaySoldierDialogue(TArray<USoldierDialogueComponent*> SourceList, TriggerEventType Trigger, bool IsStressed);

	void OnWorldBeginPlay(UWorld& InWorld);

	static void CallbackTest(AkCallbackType In_eType, AkCallbackInfo* In_pCallbackInfo);

	UFUNCTION()
	void NextLine(UDialogueSession* DialogueSession);

protected:
	struct CallbackInfo
	{
		UDialogueSubsystem* DS;
		UDialogueSession* DialogueSession;
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDialogueDataAsset* DialogueDataAsset;

	TMap<UDialogueSession*, TTuple<TArray<USoldierDialogueComponent*>, int>> Dialogues = TMap<UDialogueSession*, TTuple<TArray<USoldierDialogueComponent*>, int>>(); // int = IdxLine of Dialogue

	int DialogueLineIdx = 0;

	FOnAkPostEventCallback BindCallback;

	UDialogueSession* FindDialogue(int MinInterlocutors, int MaxInterlocutors);

	UDialogueSession* FindDialogue(int MinInterlocutors, int MaxInterlocutors, BehaviorStates From, BehaviorStates To);

	UDialogueSession* FindDialogue(int MinInterlocutors, int MaxInterlocutors, BehaviorStates From, BehaviorStates To, bool IsStressed);

	UDialogueSession* FindDialogue(int MinInterlocutors, int MaxInterlocutors, TriggerEventType Trigger);

	UDialogueSession* FindDialogue(int MinInterlocutors, int MaxInterlocutors, TriggerEventType Trigger, bool IsStressed);

	void AddDialogue(UDialogueSession* DialogueSession, TArray<USoldierDialogueComponent*>& SourceList);

	void StartLine(UDialogueSession* DialogueSession);
};
