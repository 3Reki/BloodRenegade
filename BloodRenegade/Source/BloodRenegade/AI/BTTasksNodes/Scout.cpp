// Fill out your copyright notice in the Description page of Project Settings.


#include "Scout.h"
#include "../Soldier.h"
#include "../GOAPController.h"

UScout::UScout() 
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
}


//TICK TASK TOO COSTLY
void UScout::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	TWeakObjectPtr<AAIController> aiController = OwnerComp.GetAIOwner();
	if (!aiController.IsValid()) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	TWeakObjectPtr<AGOAPController> goapController = Cast<AGOAPController>(aiController);
	if (!goapController.IsValid()) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	TWeakObjectPtr<ASoldier> soldier = Cast<ASoldier>(goapController->GetPawn());
	if (soldier.IsValid())
	{
		if (goapController->BlackboardComp->GetValueAsBool("EnemyVisible") || goapController->BlackboardComp->GetValueAsBool("HasSeenEnemyRecently")) FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		if (soldier->GetHasEndedPatrol()) 
		{
			soldier->Scout();
		}
	}
	else 
	{
		//UE_LOG(LogTemp, Warning, TEXT("Scout Fail 1"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

EBTNodeResult::Type UScout::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	TWeakObjectPtr<AGOAPController> goapController = Cast<AGOAPController>(OwnerComp.GetAIOwner());
	if (goapController.IsValid()) 
	{
		TWeakObjectPtr<ASoldier> soldier = Cast<ASoldier>(goapController->GetPawn());
		if (soldier.IsValid()) 
		{
			//UE_LOG(LogTemp, Warning, TEXT("Scout"));
			if (soldier->Scout())
			{
				return EBTNodeResult::InProgress;
			}
			soldier->ChangePlan();
			//UE_LOG(LogTemp, Warning, TEXT("Scout Fail Start 1"));
			return EBTNodeResult::Failed;
		}
		//UE_LOG(LogTemp, Warning, TEXT("Scout Fail Start 2"));
		return EBTNodeResult::Failed;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Scout Fail Start 3"));
	return EBTNodeResult::Failed;
}