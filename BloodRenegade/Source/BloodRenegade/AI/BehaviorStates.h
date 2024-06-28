// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorStates.generated.h"


UENUM(BlueprintType)
enum class BehaviorStates : uint8
{
	Alert, Freeze, Neutral, Search, Suspect
};
