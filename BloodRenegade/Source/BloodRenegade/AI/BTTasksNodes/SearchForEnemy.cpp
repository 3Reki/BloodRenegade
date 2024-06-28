// Fill out your copyright notice in the Description page of Project Settings.


#include "SearchForEnemy.h"
#include "../Soldier.h"
#include "../SoldierDialogueComponent.h"
#include "../FearComponent.h"
#include "../GOAPController.h"

USearchForEnemy::USearchForEnemy() 
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
}

void USearchForEnemy::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	TWeakObjectPtr<AGOAPController> goapController = Cast<AGOAPController>(OwnerComp.GetAIOwner());
	if (goapController.IsValid()) 
	{
		TWeakObjectPtr<ASoldier> soldier = Cast<ASoldier>(goapController->GetPawn());
		if (soldier.IsValid()) 
		{
			FName planStateChangedKey = FName(TEXT("PlanStateChanged"));
			FName enemyVisibleKey = FName(TEXT("EnemyVisible"));
			FName hasSeenEnemyRecentlyKey = FName(TEXT("HasSeenEnemyRecently"));

			if (soldier->GetDetectionPercentage() > 100 
				&& ((!soldier->GetTargetStimuliActors().IsEmpty() && !soldier->GetSoundProcessSFE())
					|| soldier->GetIsInEnemySight()))
			{
				soldier->SearchForEnemy();
			}
			if (goapController->BlackboardComp->GetValueAsBool(enemyVisibleKey)) 
			{
				//UE_LOG(LogTemp, Warning, TEXT("Search For Enemy Success"));
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
			if (soldier->GetHasFinishedMovement() && soldier->GetHasEndedSearch())
			{
				//UE_LOG(LogTemp, Warning, TEXT("Search For Enemy Success fail 1"));
				goapController->BlackboardComp->SetValueAsBool(planStateChangedKey, true);
				goapController->SetActualWorldStateAtomValue("HasSeenEnemyRecently", false);
				soldier->ChangePlan();
				FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			}
		}
	}
}

EBTNodeResult::Type USearchForEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	TWeakObjectPtr<AGOAPController> goapController = Cast<AGOAPController>(OwnerComp.GetAIOwner());
	if (goapController.IsValid()) 
	{
		
		TWeakObjectPtr<ASoldier> soldier = Cast<ASoldier>(goapController->GetPawn());
		FName enemyVisibleKey = FName(TEXT("EnemyVisible"));
		
		if (soldier.IsValid()) 
		{
			//UE_LOG(LogTemp, Warning, TEXT("Search For Enemy"));
			if (goapController->BlackboardComp->GetValueAsBool(enemyVisibleKey))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Search For Enemy Success Start 1"));
				return EBTNodeResult::Succeeded;
			}
			if (soldier->GoapController.Get()->BlackboardComp->GetValueAsBool("WasAlerted"))
			{
				soldier->DialogueComponent->PlayDialogue(BehaviorStates::Alert, BehaviorStates::Search);
			}
			if (soldier->SearchForEnemy())
			{
				return EBTNodeResult::InProgress;
			}
			else 
			{
				//UE_LOG(LogTemp, Warning, TEXT("Search For Enemy fail Start"));
				FName planStateChangedKey = FName(TEXT("PlanStateChanged"));
				goapController->BlackboardComp->SetValueAsBool(planStateChangedKey, true);
				goapController->SetActualWorldStateAtomValue("EnemyVisible", false);
				goapController->SetActualWorldStateAtomValue("HasSeenEnemyRecently", false);
				soldier->ChangePlan();
				return EBTNodeResult::Failed;
			}
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Search For Enemy Success ultra fail"));
	return EBTNodeResult::Failed;
}