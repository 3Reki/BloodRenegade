// Fill out your copyright notice in the Description page of Project Settings.

#include "RunPlanner.h"
#include "../GOAPController.h"

EBTNodeResult::Type URunPlanner::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TWeakObjectPtr<AGOAPController> GOAPController = Cast<AGOAPController>(OwnerComp.GetAIOwner());
	if (GOAPController.IsValid())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Run Planner"));
		return (GOAPController->GetNewPlan()) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Failed;
}


