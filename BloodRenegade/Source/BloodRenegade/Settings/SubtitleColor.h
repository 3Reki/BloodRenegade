#pragma once

#include "CoreMinimal.h"
#include "SubtitleColor.generated.h"

UENUM(BlueprintType)
enum class ESubtitleColor : uint8
{
	White		UMETA(DisplayName="White"),
	Black		UMETA(DisplayName="Black"),
	Yellow		UMETA(DisplayName="Yellow"),
	Blue		UMETA(DisplayName="Blue"),
	Red			UMETA(DisplayName="Red"),
	Green		UMETA(DisplayName="Green"),
};
