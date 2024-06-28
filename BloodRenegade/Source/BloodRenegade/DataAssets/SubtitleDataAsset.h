#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SubtitleDataAsset.generated.h"

class UAkAudioEvent;

USTRUCT(BlueprintType)
struct FSubtitle 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAkAudioEvent* VoiceLine;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Subtitle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DelayBeforeVoiceLine = 0.0f;
};

UCLASS(BlueprintType)
class BLOODRENEGADE_API USubtitleDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSubtitle> Subtitles;
};
