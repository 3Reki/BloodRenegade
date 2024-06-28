// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericGraph.h"
#include "../AI/TriggerEventType.h"
#include "DialogueSession.generated.h"

/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UDialogueSession : public UGenericGraph
{
	GENERATED_BODY()

public:
    UDialogueSession();

    bool CanPlayDialogue();

    void ResetRecentActivation();

    UPROPERTY(EditAnywhere)
    int NBInterlocutors;

    UPROPERTY(EditAnywhere, meta = (ClampMin = "0", ClampMax = "100"))
    int ActivationPercentage = 100;

protected:
    int RecentActivation = 0;
    int ActivationIdx = 0;
};
