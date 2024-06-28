// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Strike.generated.h"

class AGOAPController;
/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UStrike : public UBTTaskNode
{
	GENERATED_BODY()

public:
	
	UStrike();

	void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected :

	AGOAPController* GoapController;

};
