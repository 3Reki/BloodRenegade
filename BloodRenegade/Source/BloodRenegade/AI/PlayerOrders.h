// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerOrders.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EPlayerOrders : uint8
{
	Suicide		UMETA(DisplayName="Suicide"),
	Flee		UMETA(DisplayName="Flee"),
	None		UMETA(DisplayName="None")
};
