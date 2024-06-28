// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Materials/MaterialInstance.h"
#include "WallMaterialDataAsset.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class BLOODRENEGADE_API UWallMaterialDataAsset : public UDataAsset
{
	GENERATED_BODY()

public :	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialInstance*> Materials;
};
