// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "BloodCharacter.generated.h"

class URectLightComponent;
class UNightmareFormComponent;
class UCustomSpringArm;
class UStaminaComponent;
class UCustomCameraComponent;
class UVibationDataAsset;
class UVampiricVisionComponent;
class UCharacterSoundDataAsset;
class UHideSystem;
class UClimbSystem;
class UHookSystem;
class ABloodPlayerState;
class IGenericTeamAgentInterface;
class UHealthSystem;
class UBloodCharacterDataAsset;
class UBiteSystem;

UCLASS()
class BLOODRENEGADE_API ABloodCharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PostProcess, meta = (AllowPrivateAccess = "true"))
	UPostProcessComponent* PostProcessVolume;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variables|Data")
	UBloodCharacterDataAsset* DataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variables|Data");
	UVibationDataAsset* VibrationDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variables|Data")
	UCharacterSoundDataAsset* SoundDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UHookSystem* HookSystemNew;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Blueprintable)
	UBiteSystem* BiteSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UHealthSystem* HealthSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UHideSystem* HideSystem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UVampiricVisionComponent* NewVampiricVisionComponent;

	UPROPERTY(Blueprintable, EditAnywhere, BlueprintReadWrite)
	UClimbSystem* ClimbSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* VisiblePoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URectLightComponent* RimLight;

	UPROPERTY(Blueprintable, EditAnywhere, BlueprintReadOnly)
	UAnimInstance* CurrentAnims;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCustomSpringArm* CameraStick;

	UPROPERTY(BlueprintReadOnly)
	TSoftObjectPtr<ABloodPlayerState> CurrentPlayerState;

	UPROPERTY(EditAnywhere)
	UAnimMontage* HardJumpRecoveryMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* InteractAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Blueprintable)
	float BlendMaskPercent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY()
	FTimeline ModifFovTimeline;
	
	ECollisionChannel TraceChannel;
	FCollisionQueryParams ParamRaycast;
	bool BloodGaugeEnable, CooldownEnable, CanMoveWhileHiding, IsOnGround, WasGrounded, IsDead;
	float SpeedModifier, CapsuleRadius, CapsuleHeight, FallVelocity;
	FVector2D InteractScreenLoc;
	
	DECLARE_DELEGATE_OneParam(FEventCanBeStrike, bool /* b */)
	FEventCanBeStrike CanBeStrike;

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void UpdateChangeForm(float f);
	
	UFUNCTION(Blueprintable, BlueprintCallable)
	void TakeHit(int dmg);
	
	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void TakeHitEffect(bool b);

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void FinisherBite();

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void StartVampiricVisionEffect();
	
	UFUNCTION(BlueprintCallable)
	void Stunned(float f);

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void CancelVampiricVisionEffect();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void BloodSplash();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float XDirection;

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void EffectDash();

	UFUNCTION(BlueprintCallable)
	void SetCantMove(bool b);

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void EffectBeforeHook();

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void EffectAfterHook();

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void EffectTargetBlindLock(FVector Position);
	
	UFUNCTION(Blueprintable, BlueprintImplementableEvent, BlueprintCallable)
	void EffectScanVampiricVision();

	UFUNCTION(Blueprintable, BlueprintImplementableEvent, BlueprintCallable)
	void EffectStartNightmareForm();

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void EffectEndNightmareForm();

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void EffectDeath();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void EffectMovementCamera(bool Set, FVector Position, FRotator Rotator);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void LoadPlayerSave();

	UFUNCTION(Blueprintable, BlueprintImplementableEvent, BlueprintCallable)
	void SetBlendMask(bool b);

	UFUNCTION(BlueprintCallable)
	void SetBloodGauge(int GaugeValue);

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void SetBiteEffects(bool b);

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void InLightEffect(bool InLight);
	
	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetVampiricSphereMesh();

	UFUNCTION(BlueprintCallable)
	void PlayFootstepSound();

	UFUNCTION(BlueprintCallable)
	void SetCurrentInteractable(AActor* InteractableActor);

	UFUNCTION(BlueprintCallable)
	void UnsetInteractable(AActor* InteractableActor);

	UFUNCTION(BlueprintCallable)
	void SetCrouch(bool b);

	UFUNCTION(BlueprintCallable)
	void StartNightmareForm(int f);

	UFUNCTION(BlueprintCallable)
	void SetActiveNightmareForm();

	UFUNCTION(BlueprintCallable)
	void CheckEndGame(int i);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void StartEndGame();
	
	ABloodCharacter();
	void Move(FVector2D _dir);
	void Look(FVector2D);
	void Fly(float Dir);
	void SetModifier(float f);
	void Death();
	float GetCurrentStateSpeed() const;
	void StartJumpRecovery();
	void EndJumpRecovery();
	void Interact();
	virtual void ClientCheatWalk_Implementation() override;
	virtual void ClientCheatGhost_Implementation() override;
	virtual void Jump() override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	bool IsGrounded();
	void ActivateSkill();
	void CancelSkill();
	void StartTranslateCharacter(FVector start, FVector destination, float Timing);
	void SetMaxFOV(float f);
	void EnemiesAlertChanged(bool IsAlerted);
	void EnemiesSuspiciousChanged(bool IsSuspicious);
	void SetActiveEscort(bool b);
	bool GetIsSeenBySoldier();
protected:	
	UPROPERTY(EditAnywhere)
	FGenericTeamId TeamId;

	UPROPERTY()
	FTimeline TimelineLerpCrouch;
	
	UFUNCTION()
	void LerpFOVCamera(float val);

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void KillEffects();
	
	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void StrunEffects();
	
	UFUNCTION()
	void CoroutineCrouch(float Value);

	UPROPERTY(EditAnywhere)
	UAnimMontage* HookMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* HitMontage;
private:

	UPROPERTY()
	FTimeline TranslatePlayerTimeline;

	FVector StartRimLocation;
	float LastTiming;
	bool IsCoyoteTime, WasCrouched, IsSeenBySoldier, CanPlayAlertSound;
	int RayCounter;
	FHitResult RaycastResult;
	FTimerHandle JumpRecoveryTimerHandle, StrikeTimerHandle, StrikeRecoveryTimerHandle, CoyoteTimerHandle;
	FTimerHandle TimerHandleCamPosition, TimerHandleCamRotation;
	UCurveVector* TranslatePositionCurve;
	UCurveFloat *ModifFovCurv;
	float InitialFOV, MaxFOV;
	EPhysicalSurface CurrentSurfaceType;
	
	void EnableCanBeStrike();
	void UnStunned();
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void LerpPlayerPosition(FVector value);

	UFUNCTION()
	void FinishTranslate();
};
