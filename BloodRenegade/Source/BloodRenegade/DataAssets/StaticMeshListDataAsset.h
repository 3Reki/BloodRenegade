// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StaticMeshListDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BLOODRENEGADE_API UStaticMeshListDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UStaticMesh*> StaticMeshes;
};
