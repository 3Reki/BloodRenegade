// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AStar.h"
#include "GOAP.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GenericTeamAgentInterface.h"
#include "GOAPController.generated.h"

/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API AGOAPController : public AAIController
{
	GENERATED_BODY()
	
	AGOAPController(const class FObjectInitializer& ObjectInitializer);

	AStar AStarPlanner;
	GOAP GOAPPlanner;


public:
	//Action articulator
	UPROPERTY(BlueprintReadWrite)
	UBehaviorTreeComponent* BehaviorComp;
	UPROPERTY(BlueprintReadWrite)
	UBlackboardComponent* BlackboardComp;

	//Planning
	bool GetNewPlan();
	void SetActualPlanCost(const FName* PlanCostBlackboardKey, int32 PlanCost);

	//Storage
	TMap<FName, const char>* ActionNames;
	TMap<FName, int32>* ActionValues;
	TMap<FName, const char>* ImportantAtoms;
	TMap<FName, TArray<TTuple<FName, bool>>>* Goals;
	FName* actual_goal_map_key;
	FName* last_goal_map_key;

	//Actions (Values)
	int32 GetActionValue(const FName* ActionName);
	void SetActionValue(const FName* ActionName, int32 Value);
	void SetActionCost(const FName* ActionName, int32 Cost);
	void ResetActionValues();
	void ResetActionValue(const FName* ActionName);
	bool ResetLastSuccessfulAction();
	int32 GetActualActionsUtilitySum();

	//Atoms (States)
	bool GetAtomState(const FName* ActionName);
	void SetAtomState(const FName* ActionName, bool value);

	//Actions pre/pst
	void SetActionPre(const char* ActionName, const char* RelatedKey, bool RelatedKeyStateValue);
	void SetActionPst(const char* ActionName, const char* RelatedKey, bool RelatedKeyStateValue);

	//WorldState(actual/desired)
	void GetActualWorldstate(GOAP::worldstate_t* ActualWorldState);
	void SetActualWorldStateAtomValue(const char* AtomName, bool AtomValue);
	void SetDesiredWorldStateAtomValue(const char* AtomName, bool AtomValue);
	void SetDesiredWorldstate(GOAP::worldstate_t* GoalWorldstate);

	void DebugDesiredWS();

	//Goal Management
	bool CheckGoal();

	//Components (BT/BB)
	FORCEINLINE UBehaviorTreeComponent* GetBehaviorComp() const { return BehaviorComp; };
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return BlackboardComp; };

	UFUNCTION(BlueprintCallable)
	void StopBehaviorTree();
	UFUNCTION(BlueprintCallable)
	void RestartBehaviorTree();

	//AIPerception
	ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& other) const override;


protected:
	GOAP::actionplanner_t ActionPlanner;
	GOAP::worldstate_t ActualWorldState;
	GOAP::worldstate_t DesiredWorldState;
	void DestroyControlledCharacter();

	virtual void OnPossess(class APawn* InPawn) override;
	virtual void OnUnPossess() override;

};
