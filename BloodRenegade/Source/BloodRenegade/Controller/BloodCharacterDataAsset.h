// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BloodCharacterDataAsset.generated.h"

class AStaticMeshActor;

UCLASS(BlueprintType)
class BLOODRENEGADE_API UBloodCharacterDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = Speed)
	float WalkSpeed;
	
	UPROPERTY(EditAnywhere, Category = Speed)
	float CrouchSpeed;
	
	UPROPERTY(EditAnywhere, Category = Speed)
	float EscortSpeed = 200;
	
	UPROPERTY(EditAnywhere, Category = Bite)
	float BiteGroundRange = 150;

	UPROPERTY(EditAnywhere, Category = Bite)
	float BiteGroundQTERange = 150;

	UPROPERTY(EditAnywhere, Category = Bite)
	float BiteAirQTERange = 300;
	
	UPROPERTY(EditAnywhere, Category = Bite)
	float DurationQTE = 2;

	UPROPERTY(EditAnywhere, Category = Bite)
	float DilatationTimeQTE = 0.2;

	UPROPERTY(EditAnywhere, Category = Bite)
	float MinPercentQTE = 0.6;

	UPROPERTY(EditAnywhere, Category = Bite)
	float BiteGroundWidth = 50;

	UPROPERTY(EditAnywhere, Category = Bite)
	float BiteAirRange = 75;

	UPROPERTY(EditAnywhere, Category = Bite)
	float BiteAirWidth = 75;
	
	UPROPERTY(EditAnywhere, Category = Bite)
	float BiteAirHeight = 100;
	
	UPROPERTY(EditAnywhere, Category = Bite)
	float StunEnemyTimer;
	
	UPROPERTY(EditAnywhere, Category = Bite)
	UCurveFloat* BiteLedgeMovement;

	UPROPERTY(EditAnywhere, Category=Bite)
	float BiteTiming;

	UPROPERTY(EditAnywhere, Category=Bite)
	float BiteQteRangeMinOpacity = 0.3f;

	UPROPERTY(EditAnywhere, Category=Bite)
	float BiteQteRangeMaxOpacity = 0.9f;

	UPROPERTY(EditAnywhere, Category=Bite)
	float ShortBiteSoundRange = 500;

	UPROPERTY(EditAnywhere, Category=Bite)
	float OnLedgeBiteSpeed = 0.35f;

	UPROPERTY(EditAnywhere, Category = InteracteWithObject)
	float TimeToInteract;
	
	UPROPERTY(EditAnywhere, Category = Skill)
	float SkillPerimeterRange;

	UPROPERTY(EditAnywhere, Category = Skill)
	float SkillPerimeterMaxRange = 2000;

	UPROPERTY(EditAnywhere, Category = Skill)
	float BlindEnemyTimer = 5;

	UPROPERTY(EditAnywhere, Category=Skill)
	int MaxBloodGauge;

	UPROPERTY(EditAnywhere, Category=Skill)
	float TimerForAddTargetToBlind = 2;

	UPROPERTY(EditAnywhere, Category=Skill)
	float TimerAddFirstBlood = 2;

	UPROPERTY(EditAnywhere, Category=Skill)
	float TimerDecreaseBlood = 0.75f;

	UPROPERTY(EditAnywhere, Category=Skill)
	float TimerDilatationTime = 6.0f;

	UPROPERTY(EditAnywhere, Category=AIM)
	float MouseThreshold;

	UPROPERTY(EditAnywhere, Category=LifePoint)
	int MaxHp;

	UPROPERTY(EditAnywhere, Category=LifePoint)
	int TimerInvincible = 3;
	
	UPROPERTY(EditAnywhere, Category=LifePoint)
	float CooldownHP;

	UPROPERTY(EditAnywhere,  Category = Crouch)
	UCurveFloat* CrouchCurve;

	UPROPERTY(EditAnywhere,  Category = Crouch)
	UCurveFloat* CameraCurve;

	UPROPERTY(EditAnywhere,  Category = Hook)
	float HookMinimumDistance = 200;
	
	UPROPERTY(EditAnywhere,  Category = Hook)
	float HookCooldown = 0.5;

	UPROPERTY(EditAnywhere,  Category = Hook)
	float HookRange = 1200;

	UPROPERTY(EditAnywhere,  Category = Hook)
	float HookMaxVisible = 1500;

	UPROPERTY(EditAnywhere,  Category = Hook)
	float HookSpeed = 0.35f;
	
	UPROPERTY(EditAnywhere,  Category = Dash)
	float DashCooldown;

	UPROPERTY(EditAnywhere,  Category = Dash)
	float DashStrength = 2000;

	UPROPERTY(EditAnywhere,  Category = Fall)
	float FallVelocityForRecover;

	UPROPERTY(EditAnywhere,  Category = Fall)
	float TimeForRecover;

	UPROPERTY(EditAnywhere, Category = Sound)
	float FallSoundRange;

	UPROPERTY(EditAnywhere, Category = Sound)
	float StepSoundRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	float InteractSoundRange;

	UPROPERTY(EditAnywhere,  Category = VampiricVision)
	float TimerForVampiricVision = 3;

	UPROPERTY(EditAnywhere,  Category = VampiricVision)
	float RadiusForVampiricVision;

	UPROPERTY(EditAnywhere,  Category = VampiricVision)
	TSubclassOf<AStaticMeshActor> SphereMesh;

	UPROPERTY(EditAnywhere,  Category = State)
	float StrikeRecoveryTimer;

	UPROPERTY(EditAnywhere,  Category = Stamina)
	float DashResource = 25;

	UPROPERTY(EditAnywhere,  Category = Stamina)
	float HookResource = 25;
	
	UPROPERTY(EditAnywhere,  Category = Stamina)
	float VampireVisionResource = 25;
	
	UPROPERTY(EditAnywhere,  Category = Stamina)
	float MaxStamina = 100;

	UPROPERTY(EditAnywhere,  Category = Stamina)
	float BufferWaitStaminaTimer = 0.75f;

	UPROPERTY(EditAnywhere,  Category =Curve)
	UCurveFloat* TeleportAccelerationCurve;

	UPROPERTY(EditAnywhere,  Category = Climb)
	float ClimbSpeed;

	UPROPERTY(EditAnywhere,  Category = Vault)
	float VaultHeight = 120;

	UPROPERTY(EditAnywhere,  Category = Vault)
	float VaultLength = 200;
	
	UPROPERTY(EditAnywhere,  Category = Vault)
	float VaultSpeed = 0.2f;

	UPROPERTY(EditAnywhere,  Category = CameraShake)
	TSubclassOf<UCameraShakeBase> CameraShakeTakeDamage;

	UPROPERTY(EditAnywhere,  Category = CameraShake)
	TSubclassOf<UCameraShakeBase> CameraShakeStrike;

	UPROPERTY(EditAnywhere,  Category = CameraShake)
	TSubclassOf<UCameraShakeBase> CameraShakeStartBite;

	UPROPERTY(EditAnywhere,  Category = CameraShake)
	TSubclassOf<UCameraShakeBase> CameraShakeEndLongBite;
};
