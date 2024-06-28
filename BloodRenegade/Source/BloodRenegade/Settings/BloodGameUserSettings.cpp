#include "BloodGameUserSettings.h"

#include "AkGameplayStatics.h"
#include "SubtitleColor.h"
#include "SubtitleSize.h"

UBloodGameUserSettings* UBloodGameUserSettings::GetBloodGameUserSettings()
{
	return static_cast<UBloodGameUserSettings*>(GEngine->GetGameUserSettings());
}

void UBloodGameUserSettings::SetToDefaults()
{
	Super::SetToDefaults();

	ControllerXSensitivity = 1.0f;
	ControllerYSensitivity = 1.0f;
	MouseXSensitivity = 1.0f;
	MouseYSensitivity = 1.0f;
	ControllerVibrationEnabled = true;
	Brightness = 0.0f;
	ColorVisionDeficiencyMode = EColorVisionDeficiency::NormalVision;
	ColorVisionDeficiencyIntensity = 1.0f;

	DisplaySubtitles = true;
	SubtitleSize = ESubtitleSize::Medium;
	SubtitleColor = ESubtitleColor::White;
	SubtitleBackgroundColor = ESubtitleColor::Black;
	SubtitleBackgroundOpacity = 70.0f;

	MappedKeyInfos.Empty();
	AudioBusVolumes.Empty();
}

void UBloodGameUserSettings::ApplyBloodSettings() const
{
	const float AdjustedIntensity = ColorVisionDeficiencyIntensity * 10.0f;
	FSlateApplicationBase::Get().GetRenderer()->SetColorVisionDeficiencyType(ColorVisionDeficiencyMode, AdjustedIntensity, true, false);

	for (const TTuple<FName, float>& AudioBusVolume : AudioBusVolumes)
	{
		UAkGameplayStatics::SetRTPCValue(nullptr, AudioBusVolume.Value, 0, nullptr, AudioBusVolume.Key);
	}
}

float UBloodGameUserSettings::GetControllerXSensitivity() const
{
	return ControllerXSensitivity;
}

void UBloodGameUserSettings::SetControllerXSensitivity(const float Sensitivity)
{
	ControllerXSensitivity = Sensitivity;
}

float UBloodGameUserSettings::GetControllerYSensitivity() const
{
	return ControllerYSensitivity;
}

void UBloodGameUserSettings::SetControllerYSensitivity(const float Sensitivity)
{
	ControllerYSensitivity = Sensitivity;
}

float UBloodGameUserSettings::GetMouseXSensitivity() const
{
	return MouseXSensitivity;
}

void UBloodGameUserSettings::SetMouseXSensitivity(const float Sensitivity)
{
	MouseXSensitivity = Sensitivity;
}

float UBloodGameUserSettings::GetMouseYSensitivity() const
{
	return MouseYSensitivity;
}

void UBloodGameUserSettings::SetMouseYSensitivity(const float Sensitivity)
{
	MouseYSensitivity = Sensitivity;
}
