// Fill out your copyright notice in the Description page of Project Settings.


#include "SoldierDialogueComponent.h"
#include "../Subsystems/World/AIManagerSubsystem.h"
#include "../Subsystems/World/DialogueSubsystem.h"
#include "Components/AudioComponent.h"
#include "AkAudioEvent.h"
#include "AkComponent.h"
#include "Soldier.h"

// Sets default values for this component's properties
USoldierDialogueComponent::USoldierDialogueComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USoldierDialogueComponent::PlaySound()
{
	//FOnAkPostEventCallback NullCallback;
	//EventID = UAkGameplayStatics::PostEvent(DialogueEvent, GetOwner(), 0, NullCallback);
}

void USoldierDialogueComponent::StopSound() 
{
	//DialogueEvent->ExecuteAction(AkActionOnEventType::Stop, GetOwner(), EventID, 0);
}

// Called when the game starts
void USoldierDialogueComponent::BeginPlay()
{
	Super::BeginPlay();

	SDCInRange = TArray<USoldierDialogueComponent*>();
	FOnAkPostEventCallback callback;
}

void USoldierDialogueComponent::FindInterlocutors()
{
	UAIManagerSubsystem* AISubsystem = GetWorld()->GetSubsystem<UAIManagerSubsystem>();
	ASoldier* owner = Cast<ASoldier>(GetOwner());
	SDCInRange = TArray<USoldierDialogueComponent*>();
	SDCInRange.Add(this);
	TArray<ASoldier*> CloseSoldiers = AISubsystem->GetAllSoldiersInRange(GetOwner()->GetActorLocation(), InterlocutorsDistance);
	for (int i = 0; i < CloseSoldiers.Num(); i++)
	{
		SDCInRange.AddUnique(CloseSoldiers[i]->DialogueComponent);
	}
}

void USoldierDialogueComponent::PlayDialogue(BehaviorStates From, BehaviorStates To) 
{
	FindInterlocutors();

	UDialogueSubsystem* DialogueSubsystem = GetWorld()->GetSubsystem<UDialogueSubsystem>();
	DialogueSubsystem->PlaySoldierDialogue(SDCInRange, From, To);
}


void USoldierDialogueComponent::PlayDialogue(TriggerEventType Trigger) 
{
	FindInterlocutors();

	UDialogueSubsystem* DialogueSubsystem = GetWorld()->GetSubsystem<UDialogueSubsystem>();
	DialogueSubsystem->PlaySoldierDialogue(SDCInRange, Trigger);
}


