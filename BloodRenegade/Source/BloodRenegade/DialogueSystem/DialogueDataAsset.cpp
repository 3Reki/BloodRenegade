// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogueDataAsset.h"
#include "DialogueSession.h"
#include "DialogueSessionSoldier.h"


void UDialogueDataAsset::ResetAllDialoguesActivation()
{
	for (int i = 0; i < Dialogues.Num(); i++) 
	{
		Dialogues[i]->ResetRecentActivation();
	}
}

TArray<UDialogueSession*> UDialogueDataAsset::GetDialogues(int MinInterlocutors, int MaxInterlocutors)
{
	TArray<UDialogueSession*> Result = TArray<UDialogueSession*>();
	for (int i = 0; i < Dialogues.Num(); i++) 
	{
		if (Dialogues[i]->NBInterlocutors >= MinInterlocutors && Dialogues[i]->NBInterlocutors <= MaxInterlocutors) 
		{
			Result.Add(Dialogues[i]);
		}
	}
	return Result;
}

TArray<UDialogueSession*> UDialogueDataAsset::GetDialoguesSoldier(int MinInterlocutors, int MaxInterlocutors, TriggerEventType Trigger) 
{
	TArray<UDialogueSession*> Result = TArray<UDialogueSession*>();
	for (int i = 0; i < SoldierDialogues.Num(); i++)
	{
		if (SoldierDialogues[i]->NBInterlocutors >= MinInterlocutors
			&& SoldierDialogues[i]->NBInterlocutors <= MaxInterlocutors
			&& SoldierDialogues[i]->Trigger == Trigger)
		{
			Result.Add(SoldierDialogues[i]);
		}
	}
	return Result;
}

TArray<UDialogueSession*> UDialogueDataAsset::GetDialoguesSoldier(int MinInterlocutors, int MaxInterlocutors, TriggerEventType Trigger, bool IsStressed) 
{
	TArray<UDialogueSession*> Result = TArray<UDialogueSession*>();
	for (int i = 0; i < SoldierDialogues.Num(); i++) 
	{
		if (	SoldierDialogues[i]->NBInterlocutors >= MinInterlocutors
			&&	SoldierDialogues[i]->NBInterlocutors <= MaxInterlocutors
			&&	SoldierDialogues[i]->Trigger == Trigger
			&&	SoldierDialogues[i]->IsStressed == IsStressed)
		{
			Result.Add(SoldierDialogues[i]);
		}
	}
	return Result;
}

TArray<UDialogueSession*> UDialogueDataAsset::GetDialoguesSoldier(int MinInterlocutors, int MaxInterlocutors, BehaviorStates From, BehaviorStates To) 
{
	TArray<UDialogueSession*> Result = TArray<UDialogueSession*>();
	for (int i = 0; i < SoldierDialogues.Num(); i++)
	{
		if (SoldierDialogues[i]->NBInterlocutors >= MinInterlocutors
			&& SoldierDialogues[i]->NBInterlocutors <= MaxInterlocutors
			&& SoldierDialogues[i]->StateFrom == From
			&& SoldierDialogues[i]->StateTo == To)
		{
			Result.Add(SoldierDialogues[i]);
		}
	}
	return Result;
}

TArray<UDialogueSession*> UDialogueDataAsset::GetDialoguesSoldier(int MinInterlocutors, int MaxInterlocutors, BehaviorStates From, BehaviorStates To, bool IsStressed) 
{
	TArray<UDialogueSession*> Result = TArray<UDialogueSession*>();
	for (int i = 0; i < SoldierDialogues.Num(); i++) 
	{
		if (	SoldierDialogues[i]->NBInterlocutors >= MinInterlocutors
			&&	SoldierDialogues[i]->NBInterlocutors <= MaxInterlocutors
			&&	SoldierDialogues[i]->StateFrom == From
			&&	SoldierDialogues[i]->StateTo == To
			&&	SoldierDialogues[i]->IsStressed == IsStressed)
		{
			Result.Add(SoldierDialogues[i]);
		}
	}
	return Result;
}