#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BloodGameInstance.generated.h"

UCLASS()
class BLOODRENEGADE_API UBloodGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void LoadComplete(const float LoadTime, const FString& MapName) override;
};
