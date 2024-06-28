// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PatrolDataAsset.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FPatrolFormation 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NBSoldier = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> Offsets = TArray<FVector>();
};

UCLASS(BlueprintType)
class BLOODRENEGADE_API UPatrolDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TArray<FPatrolFormation> PatrolFormation = TArray<FPatrolFormation>();
};
