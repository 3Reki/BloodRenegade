#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInteractable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIInteractable : public UInterface
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class EWhoCanInteract : uint8
{
	ALL,
	PLAYER_ONLY,
	SOLDIER_ONLY,
};

class BLOODRENEGADE_API IIInteractable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void Interact();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	float GetOffset();

	UFUNCTION(BlueprintImplementableEvent)
	EWhoCanInteract GetWhoCanInteract();
};