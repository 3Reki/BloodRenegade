// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataSaver.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDataSaver : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLOODRENEGADE_API IDataSaver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SaveGameData();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void LoadGameData();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ResetSaveData();
};
