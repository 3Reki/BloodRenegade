#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "BloodGameUserSettings.generated.h"

enum class ESubtitleColor : uint8;
enum class ESubtitleSize : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSubtitlesSettingsChangedSignature);

UCLASS(config=GameUserSettings, configdonotcheckdefaults, BlueprintType)
class BLOODRENEGADE_API UBloodGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category=Settings)
	static UBloodGameUserSettings* GetBloodGameUserSettings();
	
	virtual void SetToDefaults() override;

	UFUNCTION(BlueprintCallable, Category=Settings)
	void ApplyBloodSettings() const;

	UFUNCTION(BlueprintPure, Category=Settings)
	float GetControllerXSensitivity() const;
	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetControllerXSensitivity(const float Sensitivity);

	UFUNCTION(BlueprintPure, Category=Settings)
	float GetControllerYSensitivity() const;
	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetControllerYSensitivity(const float Sensitivity);

	UFUNCTION(BlueprintPure, Category=Settings)
	float GetMouseXSensitivity() const;
	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetMouseXSensitivity(float Sensitivity);

	UFUNCTION(BlueprintPure, Category=Settings)
	float GetMouseYSensitivity() const;
	UFUNCTION(BlueprintCallable, Category=Settings)
	void SetMouseYSensitivity(const float Sensitivity);

	UPROPERTY(config, BlueprintReadWrite)
	bool ControllerVibrationEnabled;

	UPROPERTY(config, BlueprintReadWrite)
	float Brightness;

	UPROPERTY(config, BlueprintReadWrite)
	EColorVisionDeficiency ColorVisionDeficiencyMode;

	UPROPERTY(config, BlueprintReadWrite)
	float ColorVisionDeficiencyIntensity;

	UPROPERTY(config, BlueprintReadWrite)
	bool DisplaySubtitles;

	UPROPERTY(config, BlueprintReadWrite)
	ESubtitleSize SubtitleSize;

	UPROPERTY(config, BlueprintReadWrite)
	ESubtitleColor SubtitleColor;

	UPROPERTY(config, BlueprintReadWrite)
	ESubtitleColor SubtitleBackgroundColor;

	UPROPERTY(config, BlueprintReadWrite)
	float SubtitleBackgroundOpacity;

	UPROPERTY(config, BlueprintReadWrite)
	TMap<FName, FKey> MappedKeyInfos;

	UPROPERTY(config, BlueprintReadWrite)
	TMap<FName, float> AudioBusVolumes;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnSubtitlesSettingsChangedSignature OnSubtitlesSettingsChangedSignature;
	
protected:

	UPROPERTY(config)
	float ControllerXSensitivity;

	UPROPERTY(config)
	float ControllerYSensitivity;
	
	UPROPERTY(config)
	float MouseXSensitivity;

	UPROPERTY(config)
	float MouseYSensitivity;
};
