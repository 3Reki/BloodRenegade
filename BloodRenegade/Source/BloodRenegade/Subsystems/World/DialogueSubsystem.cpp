// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueSubsystem.h"
#include "Components/AudioComponent.h"
#include "../../DialogueSystem/DialogueSession.h"
#include "../../DialogueSystem/DialogueSessionNode.h"
#include "../../DialogueSystem/DialogueDataAsset.h"
#include "../../AI/SoldierDialogueComponent.h"
#include "AkComponent.h"
#include "../../AI/Soldier.h"
#include <AssetRegistry/AssetRegistryModule.h>

void UDialogueSubsystem::PlayDialogue(TArray<USoldierDialogueComponent*> SourceList)
{
	UDialogueSession* DialogueSession = FindDialogue(1, SourceList.Num());

	AddDialogue(DialogueSession, SourceList);
}

void UDialogueSubsystem::PlaySoldierDialogue(TArray<USoldierDialogueComponent*> SourceList, BehaviorStates From, BehaviorStates To)
{
	UDialogueSession* DialogueSession = FindDialogue(1, SourceList.Num(), From, To);
	if (DialogueSession->CanPlayDialogue() && DialogueSession) AddDialogue(DialogueSession, SourceList);
}

void UDialogueSubsystem::PlaySoldierDialogue(TArray<USoldierDialogueComponent*> SourceList, BehaviorStates From, BehaviorStates To, bool IsStressed)
{
	UDialogueSession* DialogueSession = FindDialogue(1, SourceList.Num(), From, To, IsStressed);
	if (DialogueSession->CanPlayDialogue() && DialogueSession) AddDialogue(DialogueSession, SourceList);
}

void UDialogueSubsystem::PlaySoldierDialogue(TArray<USoldierDialogueComponent*> SourceList, TriggerEventType Trigger)
{
	UDialogueSession* DialogueSession = FindDialogue(1, SourceList.Num(), Trigger);
	if (DialogueSession->CanPlayDialogue() && DialogueSession) AddDialogue(DialogueSession, SourceList);
}

void UDialogueSubsystem::PlaySoldierDialogue(TArray<USoldierDialogueComponent*> SourceList, TriggerEventType Trigger, bool IsStressed)
{
	UDialogueSession* DialogueSession = FindDialogue(1, SourceList.Num(), Trigger, IsStressed);
	if (DialogueSession->CanPlayDialogue() && DialogueSession) AddDialogue(DialogueSession, SourceList);
}

void UDialogueSubsystem::OnWorldBeginPlay(UWorld& InWorld) 
{
	Super::OnWorldBeginPlay(InWorld);

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	const UClass* Class = UDialogueDataAsset::StaticClass();
	AssetRegistryModule.Get().GetAssetsByClass(Class->GetFName(), AssetData);

	for (FAssetData AD : AssetData) 
	{
		DialogueDataAsset = Cast<UDialogueDataAsset>(AD.GetAsset());
	}

	BindCallback.BindUFunction(this, FName("NextLine"));
	DialogueDataAsset->ResetAllDialoguesActivation();
}

UDialogueSession* UDialogueSubsystem::FindDialogue(int MinInterlocutors, int MaxInterlocutors)
{
	TArray<UDialogueSession*> DialogueSessions = DialogueDataAsset->GetDialogues(MinInterlocutors, MaxInterlocutors);
	return DialogueSessions[FMath::RandRange(0, DialogueSessions.Num() - 1)];
}

UDialogueSession* UDialogueSubsystem::FindDialogue(int MinInterlocutors, int MaxInterlocutors, BehaviorStates From, BehaviorStates To) 
{
	TArray<UDialogueSession*> DialogueSessions = DialogueDataAsset->GetDialoguesSoldier(MinInterlocutors, MaxInterlocutors, From, To);
	return DialogueSessions[FMath::RandRange(0, DialogueSessions.Num() - 1)];
}

UDialogueSession* UDialogueSubsystem::FindDialogue(int MinInterlocutors, int MaxInterlocutors, BehaviorStates From, BehaviorStates To, bool IsStressed) 
{
	TArray<UDialogueSession*> DialogueSessions = DialogueDataAsset->GetDialoguesSoldier(MinInterlocutors, MaxInterlocutors, From, To, IsStressed);
	return DialogueSessions[FMath::RandRange(0, DialogueSessions.Num() - 1)];
}

UDialogueSession* UDialogueSubsystem::FindDialogue(int MinInterlocutors, int MaxInterlocutors, TriggerEventType Trigger) 
{
	TArray<UDialogueSession*> DialogueSessions = DialogueDataAsset->GetDialoguesSoldier(MinInterlocutors, MaxInterlocutors, Trigger);
	return DialogueSessions[FMath::RandRange(0, DialogueSessions.Num() - 1)];
}

UDialogueSession* UDialogueSubsystem::FindDialogue(int MinInterlocutors, int MaxInterlocutors, TriggerEventType Trigger, bool IsStressed) 
{
	TArray<UDialogueSession*> DialogueSessions = DialogueDataAsset->GetDialoguesSoldier(MinInterlocutors, MaxInterlocutors, Trigger, IsStressed);
	return DialogueSessions[FMath::RandRange(0, DialogueSessions.Num() - 1)];
}

void UDialogueSubsystem::AddDialogue(UDialogueSession* DialogueSession, TArray<USoldierDialogueComponent*>& SourceList)
{
	if (DialogueSession != nullptr)
	{
		Dialogues.Add(DialogueSession, TTuple<TArray<USoldierDialogueComponent*>, int>(SourceList, 0));
		
		StartLine(DialogueSession);
	}
}


void UDialogueSubsystem::CallbackTest(AkCallbackType In_eType, AkCallbackInfo* In_pCallbackInfo) 
{
	CallbackInfo* Info = (CallbackInfo*)In_pCallbackInfo->pCookie;
	Info->DS->NextLine(Info->DialogueSession);

	delete Info;
}

void UDialogueSubsystem::StartLine(UDialogueSession* DialogueSession) 
{
	TTuple<TArray<USoldierDialogueComponent*>, int>* DialogueInfo = Dialogues.Find(DialogueSession);
	UDialogueSessionNode* DialogueNode = Cast<UDialogueSessionNode>(DialogueSession->AllNodes[DialogueInfo->Value]);
	
	
	UAkComponent* AkComponent;
	if (DialogueNode->SpeakerIdx >= DialogueInfo->Key.Num()) 
	{
		AkComponent = Cast<ASoldier>(DialogueInfo->Key[0]->GetOwner())->AkComponent;
	}
	else 
	{
		AkComponent = Cast<ASoldier>(DialogueInfo->Key[DialogueNode->SpeakerIdx]->GetOwner())->AkComponent;
	}

	if (!ensure(IsValid(AkComponent))) 
	{
		UE_LOG(LogTemp, Warning, TEXT("AkComponent Not Found"));
		NextLine(DialogueSession);
		return;
	}
	
	if (!DialogueInfo || !DialogueNode)
		return;
	
	AkComponent->Stop();

	const USoldierDialogueComponent* DialogueComponent = DialogueInfo->Key[DialogueNode->SpeakerIdx];
	if (!DialogueComponent)
		return;

	const UVoices SoldierVoice = DialogueComponent->SoldierVoice;
	if (!DialogueNode->DialogueLines.Contains(SoldierVoice))
		return;

	UAkAudioEvent* DialogueLine = DialogueNode->DialogueLines[SoldierVoice];
	if (!DialogueLine)
		return;
	
	CallbackInfo* CallbackInfos = new CallbackInfo{ this, DialogueSession };
	const AkPlayingID Res = AkComponent->PostAkEvent(DialogueLine, AK_EndOfEvent, UDialogueSubsystem::CallbackTest, CallbackInfos);

	if (Res == AK_INVALID_PLAYING_ID)
	{
		delete CallbackInfos;
	}
}

void UDialogueSubsystem::NextLine(UDialogueSession* DialogueSession)
{
	if (!Dialogues.Contains(DialogueSession)) return;
	int a = DialogueSession->RootNodes.Num();

	int b = Dialogues.Find(DialogueSession)->Value;
	if (b <= a)
	{
		Dialogues.Remove(DialogueSession);
		return;
	}
	Dialogues.Find(DialogueSession)->Value += 1;
	
	StartLine(DialogueSession);
}