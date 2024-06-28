// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IBitable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIBitable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLOODRENEGADE_API IIBitable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MyCategory")
	void Die();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "MyCategory")
	void Stun(float f);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BeingBite(float f);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BeingArialBite();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BeingDissolved();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BiteSound(bool IsSilent, bool PlayLongSound);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BeingAttacked();
};
