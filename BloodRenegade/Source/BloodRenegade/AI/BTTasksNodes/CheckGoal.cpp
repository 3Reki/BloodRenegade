#include "CheckGoal.h"
#include "../GOAPController.h"

EBTNodeResult::Type UCheckGoal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TWeakObjectPtr<AGOAPController> GOAPController = Cast<AGOAPController>(OwnerComp.GetAIOwner());
	if (GOAPController.IsValid()) {
		//UE_LOG(LogTemp, Warning, TEXT("Chase Player Check Goal"));
		return (GOAPController->CheckGoal()) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}
	return EBTNodeResult::Aborted;
}