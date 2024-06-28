// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterSoundDataAsset.generated.h"

class UAkSwitchValue;
class UAkAudioEvent;

UCLASS()
class BLOODRENEGADE_API UCharacterSoundDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bite", DisplayName="Short Bite")
	UAkAudioEvent* ShortBiteAudioEvent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bite", DisplayName="Long Bite")
	UAkAudioEvent* LongBiteAudioEvent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bite", DisplayName="Aerial Bite")
	UAkAudioEvent* AerialBiteAudioEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", DisplayName="Vampiric Sense Fade In")
	UAkAudioEvent* VampiricSenseFadeInAudioEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", DisplayName="Vampiric Sense Fade Out")
	UAkAudioEvent* VampiricSenseFadeOutAudioEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", DisplayName="Hook Fade Out")
	UAkAudioEvent* HookFadeOutAudioEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", DisplayName="Hook Fade In")
	UAkAudioEvent* HookFadeInAudioEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movements", DisplayName="Jump Lead")
	UAkAudioEvent* JumpLeadEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Footsteps", DisplayName="Footsteps")
	UAkAudioEvent* FootstepsAudioEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Footsteps", DisplayName="Footstep Switches")
	TMap<TEnumAsByte<EPhysicalSurface>, UAkSwitchValue*> FootstepsSwitchValues;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music", DisplayName="Music")
	UAkAudioEvent* MusicAudioEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damages", DisplayName="Take Damage")
	UAkAudioEvent* TakeDamageEvent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damages", DisplayName="Solar Burn")
	UAkAudioEvent* SolarBurnEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damages", DisplayName="Solar Burn Stop")
	UAkAudioEvent* SolarBurnStopEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", DisplayName="Blind Aim Loop")
	UAkAudioEvent* BlindAimLoop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", DisplayName="Blind Aim Loop Stop")
	UAkAudioEvent* BlindAimLoopStop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", DisplayName="Blind Invoke")
	UAkAudioEvent* BlindInvoke;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Detection", DisplayName="Enter Suspicious State")
	UAkAudioEvent* EnterSuspiciousStateEvent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Detection", DisplayName="Enter Alert State")
	UAkAudioEvent* EnterAlertStateEvent;
};
