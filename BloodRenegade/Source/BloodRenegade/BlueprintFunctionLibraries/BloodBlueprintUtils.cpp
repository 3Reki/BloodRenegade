// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodBlueprintUtils.h"

#include "BloodRenegade/Settings/SubtitleColor.h"

FString UBloodBlueprintUtils::GetAppVersion()
{
	FString AppVersion;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		AppVersion,
		GGameIni
	);

	return AppVersion;
}

void UBloodBlueprintUtils::WithEditor(bool& ExecOutput)
{
#if WITH_EDITOR
	ExecOutput = true;
#else
	ExecOutput = false;
#endif
}

void UBloodBlueprintUtils::IncrementIndexAndLoop(const TArray<int32>& TargetArray, int& Index)
{
	Index++;

	if (Index >= TargetArray.Num())
	{
		Index = 0;
	}
}

void UBloodBlueprintUtils::DecrementIndexAndLoop(const TArray<int32>& TargetArray, int& Index)
{
	Index--;

	if (Index < 0)
	{
		Index = TargetArray.Num() - 1;
	}
}

FLinearColor UBloodBlueprintUtils::ESubtitleColorToLinearColor(const ESubtitleColor& Color)
{
	switch (Color)
	{
	case ESubtitleColor::White:
		return FLinearColor(FColor::FromHex("#ffffff"));
	case ESubtitleColor::Black:
		return FLinearColor(FColor::FromHex("#010203"));
	case ESubtitleColor::Yellow:
		return FLinearColor(FColor::FromHex("#fffb37"));
	case ESubtitleColor::Blue:
		return FLinearColor(FColor::FromHex("#16fdff"));
	case ESubtitleColor::Red:
		return FLinearColor(FColor::FromHex("#ea0018"));
	case ESubtitleColor::Green:
		return FLinearColor(FColor::FromHex("#12fe06"));
	}

	checkNoEntry();
	return FLinearColor(1, 1, 1);
}
