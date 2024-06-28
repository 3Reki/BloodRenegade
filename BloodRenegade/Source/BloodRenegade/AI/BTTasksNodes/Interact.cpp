// Fill out your copyright notice in the Description page of Project Settings.


#include "Interact.h"
#include "../Soldier.h"
#include "../GOAPController.h"

UInteract::UInteract() 
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
}

void UInteract::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	
}

EBTNodeResult::Type UInteract::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TWeakObjectPtr<AGOAPController> goapController = Cast<AGOAPController>(OwnerComp.GetAIOwner());
	if (goapController.IsValid()) 
	{
		TWeakObjectPtr<ASoldier> soldier = Cast<ASoldier>(goapController->GetPawn());
		if (soldier.IsValid()) 
		{
			return (soldier->Interact() ? EBTNodeResult::InProgress : EBTNodeResult::Failed);
		}
		return EBTNodeResult::Failed;
	}
	return EBTNodeResult::Failed;
}

