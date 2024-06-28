// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootRifle.h"
#include "../Soldier.h"
#include "../GOAPController.h"

UShootRifle::UShootRifle() 
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
}

void UShootRifle::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) 
{

	TWeakObjectPtr<ASoldier> soldier = Cast<ASoldier>(OwnerComp.GetAIOwner()->GetPawn());
	if (soldier.IsValid()) 
	{
		soldier->InEnemyRange();
		soldier->RotateToPlayer();
		FName enemyInRangeKey = FName(TEXT("EnemyInRange"));
		FName enemyVisibleKey = FName(TEXT("EnemyVisible"));
		FName CanStrikeKey = FName(TEXT("CanStrike"));
		FName planStateChangedKey = FName(TEXT("PlanStateChanged"));
		if (!GoapController->BlackboardComp->GetValueAsBool(enemyInRangeKey) ||
			!GoapController->BlackboardComp->GetValueAsBool(enemyVisibleKey) ||
			GoapController->BlackboardComp->GetValueAsBool(CanStrikeKey))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Shoot Rifle Fail 1"));
			GoapController->BlackboardComp->SetValueAsBool(planStateChangedKey, true);
			soldier->StopAim();
			soldier.Get()->ChangePlan();
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
}

EBTNodeResult::Type UShootRifle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	GoapController = Cast<AGOAPController>(OwnerComp.GetAIOwner());

	TWeakObjectPtr<ASoldier> soldier = Cast<ASoldier>(GoapController->GetPawn());

	if (GoapController && soldier.IsValid())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Shoot Rifle"));
		if (soldier->Aim()) 
		{
			return EBTNodeResult::InProgress;
		}
		else 
		{
			//UE_LOG(LogTemp, Warning, TEXT("Shoot Rifle Fail Start"));
			return EBTNodeResult::Failed;
		}
	}
	//UE_LOG(LogTemp, Warning, TEXT("Shoot Rifle Ultra Fail"));
	return EBTNodeResult::Failed;
}