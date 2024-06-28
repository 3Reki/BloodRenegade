#pragma once

#include "CoreMinimal.h"
#include "SubtitleSize.generated.h"

UENUM(BlueprintType)
enum class ESubtitleSize : uint8
{
	Small		UMETA(DisplayName="Small"),
	Medium		UMETA(DisplayName="Medium"),
	Large		UMETA(DisplayName="Large"),
};
