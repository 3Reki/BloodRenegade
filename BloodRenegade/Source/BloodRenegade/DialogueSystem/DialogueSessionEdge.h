// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericGraphEdge.h"
#include "DialogueSessionEdge.generated.h"

/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UDialogueSessionEdge : public UGenericGraphEdge
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DialogueSession")
	FText Selection;
};
