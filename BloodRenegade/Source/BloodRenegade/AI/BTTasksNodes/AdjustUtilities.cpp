#include "AdjustUtilities.h"
#include "../GOAPController.h"

#ifndef PLANSTATECHANGED_BBKEY
#define PLANSTATECHANGED_BBKEY "PlanStateChanged"
#endif

EBTNodeResult::Type UAdjustUtilities::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TWeakObjectPtr<AGOAPController> GOAPController = Cast<AGOAPController>(OwnerComp.GetAIOwner());
	if (GOAPController.IsValid()) {
		GOAPController->ResetLastSuccessfulAction();
		if (GOAPController->GetActualActionsUtilitySum() == 0)
		{
			FName TaskFailedKey = FName(TEXT(PLANSTATECHANGED_BBKEY));
			GOAPController->SetAtomState(&TaskFailedKey, !GOAPController->GetAtomState(&TaskFailedKey));
		}
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Aborted;
}
