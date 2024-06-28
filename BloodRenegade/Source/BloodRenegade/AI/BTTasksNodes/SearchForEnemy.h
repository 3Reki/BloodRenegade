// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SearchForEnemy.generated.h"

/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API USearchForEnemy : public UBTTaskNode
{
	GENERATED_BODY()
	
	USearchForEnemy();

	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
