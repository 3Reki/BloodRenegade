// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IBlindable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIBlindable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLOODRENEGADE_API IIBlindable
{
	GENERATED_BODY()
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BeingBlind(float f);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	float GetBlindOffset();
};
