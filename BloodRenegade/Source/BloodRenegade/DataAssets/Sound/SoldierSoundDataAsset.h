#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoldierSoundDataAsset.generated.h"

class UAkAudioEvent;
class UAkSwitchValue;

UCLASS()
class BLOODRENEGADE_API USoldierSoundDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Footsteps", DisplayName="Footsteps")
	UAkAudioEvent* FootstepsAudioEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Footsteps", DisplayName="Footstep Switches")
	TMap<TEnumAsByte<EPhysicalSurface>, UAkSwitchValue*> FootstepsSwitchValues;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bite", DisplayName="Loud Long Bite")
	UAkAudioEvent* LoudLongBiteEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bite", DisplayName="Loud Short Bite")
	UAkAudioEvent* LoudShortBiteEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bite", DisplayName="Silent Long Bite")
	UAkAudioEvent* SilentLongBiteEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bite", DisplayName="Silent Short Bite")
	UAkAudioEvent* SilentShortBiteEvent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bite", DisplayName="Ledge Bite")
	UAkAudioEvent* LedgeBiteEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Effects", DisplayName="Shield Activation")
	UAkAudioEvent* ShieldActivationEvent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Effects", DisplayName="Shield Deactivation")
	UAkAudioEvent* ShieldDeactivationEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Effects", DisplayName="Being Blind Loop Stop")
	UAkAudioEvent* BlindLoopStop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Effects", DisplayName="Being Blind Loop")
	UAkAudioEvent* BlindLoop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skill Effects", DisplayName="Blind End")
	UAkAudioEvent* BlindEnd;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behaviour", DisplayName="Charge Shoot")
	UAkAudioEvent* ChargeShootEvent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behaviour", DisplayName="Stop Charge Shoot")
	UAkAudioEvent* ChargeShootStopEvent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Behaviour", DisplayName="Shoot")
	UAkAudioEvent* ShootEvent;
};
