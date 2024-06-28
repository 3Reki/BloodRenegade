#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LampUVDataAsset.generated.h"

UCLASS(BlueprintType)
class BLOODRENEGADE_API ULampUVDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="UV Lamp")
	float CharacterSpeedModifier = 0.25f;
	
	UPROPERTY(EditAnywhere, Category="UV Lamp")
	float TakeDamageTimer = 2.0f;

	UPROPERTY(EditAnywhere, Category="UV Lamp")
	TSubclassOf<UCameraShakeBase> CameraShakeLight;

	UPROPERTY(EditAnywhere,  Category="UV Lamp")
	float TimingDecreaseBlood = 0.75f;

	UPROPERTY(EditAnywhere,  Category="UV Lamp")
	float WaitTimerResetHp = 5.0f;

	UPROPERTY(EditAnywhere,  Category="Searching UV Lamp")
	float SpeedSearching = 4;
	
	UPROPERTY(EditAnywhere, Blueprintable, BlueprintReadOnly,  Category="Searching UV Lamp")
	float BlindTimer = 5;

	UPROPERTY(EditAnywhere, Blueprintable, BlueprintReadOnly,  Category="Searching UV Lamp")
	float BlinkingPeriod = 1;

	UPROPERTY(EditAnywhere, Blueprintable, BlueprintReadOnly, Category="Noise")
	float SoundRange = 500;
};
