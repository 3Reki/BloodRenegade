// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoldierDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BLOODRENEGADE_API USoldierDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement");
	float MaxScoutingWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement");
	float MaxSearchingWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement");
	float MaxChasingWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement");
	float MaxShootingWalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	float CentralVisionAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	float DetectionSpeedMultiplierDecoy = 1.5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	UCurveFloat* CurveCentralVisionDetectionSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	UCurveFloat* CurvePeripheralVisionMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	UCurveFloat* CurveHeightVisionMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	float PlayerCrouchedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	UCurveFloat* CurveHearingStepDetectionSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	float DetectionSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection", meta = (DisplayName = "Lose Detection Alert To Suspicious Speed"))
	float LoseDetectionATSSpeed = 33;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection", meta = (DisplayName = "Lose Detection Suspicious To Neutral Speed"))
	float LoseDetectionSTNSpeed = 33;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection", meta = (DisplayName = "Lose Detection When Not Seen"))
	float LoseDetectionNSSpeed = 33;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	float AlertRange = 2000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	float CorpseDetectionRange = 1500;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	float ForgetSoundTimer = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	UCurveFloat* CurveRotation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | SearchEnemy")
	float SearchRadius = 500;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | SearchEnemy")
	float WaitTimeOnSearchLocation = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float FireRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	int WeaponDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float AimTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float StrikeRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	int StrikeDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float StrikeStunDuration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "VFX | Aim Laser Pointer")
	FLinearColor LaserColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shield")
	float ShieldDuration = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	float ShotSoundRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI | Detection")
	float ShortBiteRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Post Process")
	float PostProcessVisionRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fear")
	float SoldierMaxFear = 200;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fear")
	float EndFearTimer = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fear")
	float StressDetectionMultiplier = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float TimerStrikePlayer = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FeedBack", meta = (ClampMin = "0", ClampMax = "200"))
	float DetectionVibrationThreshold = 25;
};
