// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BloodBlueprintUtils.generated.h"

enum class ESubtitleColor : uint8;

UENUM(BlueprintType)
enum class EExecOutput : uint8
{
	First UMETA(DisplayName = "True"),
	Second UMETA(DisplayName = "False")
};

/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UBloodBlueprintUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Project Version"), Category = "Development")
	static FString GetAppVersion();

	UFUNCTION(BlueprintCallable, Category = "Development", meta = (DisplayName = "Is Editor Build", ExpandBoolAsExecs = "ExecOutput"))
	static void WithEditor(bool& ExecOutput);

	UFUNCTION(BlueprintCallable, Category = "Utilities|Array", meta = (DisplayName = "Increment Index And Loop", ArrayParm = "TargetArray"))
	static void IncrementIndexAndLoop(const TArray<int32>& TargetArray, UPARAM(ref) int& Index);

	UFUNCTION(BlueprintCallable, Category = "Utilities|Array", meta = (DisplayName = "Decrement Index And Loop", ArrayParm = "TargetArray"))
	static void DecrementIndexAndLoop(const TArray<int32>& TargetArray, UPARAM(ref) int& Index);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "ESubtitleColor To Linear Color"), Category = "Subtitle")
	static FLinearColor ESubtitleColorToLinearColor(const ESubtitleColor& Color);
};
