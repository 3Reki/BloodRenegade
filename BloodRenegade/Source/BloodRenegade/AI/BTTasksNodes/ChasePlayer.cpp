// Fill out your copyright notice in the Description page of Project Settings.


#include "ChasePlayer.h"
#include "../Soldier.h"
#include "../GOAPController.h"

UChasePlayer::UChasePlayer() 
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
}

void UChasePlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	TWeakObjectPtr<AGOAPController> goapController = Cast<AGOAPController>(OwnerComp.GetAIOwner());

	if (goapController.IsValid()) {
		TWeakObjectPtr<ASoldier> soldier = Cast<ASoldier>(goapController->GetPawn());
		if (soldier.IsValid())
		{
			FName enemyInRangeKey = FName(TEXT("EnemyInRange"));
			FName enemyVisibleKey = FName(TEXT("EnemyVisible"));
			FName planStateChangedKey = FName(TEXT("PlanStateChanged"));

			if (!goapController->BlackboardComp->GetValueAsBool(enemyVisibleKey))
			{
				if(!soldier->GetIsInEnemySight())
				{
					//UE_LOG(LogTemp, Warning, TEXT("Chase Player Fail 1"));
					goapController->BlackboardComp->SetValueAsBool(planStateChangedKey, true);
					soldier->ChangePlan();
					FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
				}
			}
			if (soldier->InEnemyRange())
			{
				//UE_LOG(LogTemp, Warning, TEXT("Chase Player Success"));
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
			if (soldier->GetIsInEnemySight())
			{
				
				soldier->ChasePlayer();
			}
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Chase Player Fail 3"));
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
}

EBTNodeResult::Type UChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	TWeakObjectPtr<AGOAPController> GoapController = Cast<AGOAPController>(OwnerComp.GetAIOwner());
	if (GoapController.IsValid()) 
	{
		TWeakObjectPtr<ASoldier> Soldier = Cast<ASoldier>(GoapController->GetPawn());
		if (Soldier.IsValid()) 
		{
			//UE_LOG(LogTemp, Warning, TEXT("Chase Player"));
			if (Soldier->ChasePlayer()) 
			{
				return EBTNodeResult::InProgress;
			}
			else 
			{
				//UE_LOG(LogTemp, Warning, TEXT("Chase Player Fail Start"));
				FName planStateChangedKey = FName(TEXT("PlanStateChanged"));
				GoapController->BlackboardComp->SetValueAsBool(planStateChangedKey, true);
				Soldier->ChangePlan();
				return EBTNodeResult::Failed;
			}
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Chase Player Ultra Fail"));
	return EBTNodeResult::Failed;
}
