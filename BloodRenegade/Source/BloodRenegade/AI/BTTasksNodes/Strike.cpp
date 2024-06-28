// Fill out your copyright notice in the Description page of Project Settings.


#include "Strike.h"
#include "../Soldier.h"
#include "../GOAPController.h"

UStrike::UStrike()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
}

void UStrike::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{
	TWeakObjectPtr<ASoldier> Soldier = Cast<ASoldier>(OwnerComp.GetAIOwner()->GetPawn());
	if (Soldier.IsValid()) 
	{
		FName PlanStateChangedKey = FName(TEXT("PlanStateChanged"));

		if (Soldier.Get()->GetHasEndedStrike()) 
		{
			//UE_LOG(LogTemp, Warning, TEXT("Strike Fail"));
			GoapController->BlackboardComp->SetValueAsBool(PlanStateChangedKey, true);
			Soldier->ChangePlan();
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}

	}
}

EBTNodeResult::Type UStrike::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	GoapController = Cast<AGOAPController>(OwnerComp.GetAIOwner());
	TWeakObjectPtr<ASoldier> soldier = Cast<ASoldier>(GoapController->GetPawn());
	if (GoapController && soldier.IsValid()) 
	{
		//UE_LOG(LogTemp, Warning, TEXT("Strike"));
		if (soldier->Strike()) 
		{
			EBTNodeResult::InProgress;
		}
		else 
		{
			//UE_LOG(LogTemp, Warning, TEXT("Strike Fail Start"));
			EBTNodeResult::Failed;
		}  
	}
	//UE_LOG(LogTemp, Warning, TEXT("Strike Ultra Fail"));
	return EBTNodeResult::Failed;
}

