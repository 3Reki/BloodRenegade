// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "BloodController.generated.h"

class UBloodCharacterDataAsset;
class UInputMappingContext;
class UInputAction;
class UMotherSkill;
class ABloodCharacter;
class ABloodHUD;
class UBlind;
class ABloodPlayerState;
class UBloodCheatManager;
class UEnhancedInputLocalPlayerSubsystem;
class UVibationDataAsset;
/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API ABloodController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variables|Data")
	UBloodCharacterDataAsset* DataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variables|Data")
	UVibationDataAsset* VibrationDataAsset;
	
	UPROPERTY(EditDefaultsOnly, Category = Cheat)
	UInputMappingContext* NoClipMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Skill, meta = (AllowPrivateAccess = "true"))
	UBlind* BlindSkill;
	
	TWeakObjectPtr<ABloodCharacter> PossesedPawn;
	TWeakObjectPtr<ABloodPlayerState> CurrentPlayerState;
	TWeakObjectPtr<ABloodHUD> CurrentHUD;
	
	UFUNCTION(BlueprintCallable)
	void UpdateMouseSensitivityX(float Sensitivity);

	UFUNCTION(BlueprintCallable)
	void UpdateMouseSensitivityY(float Sensitivity);

	UFUNCTION(BlueprintCallable)
	void UpdateControllerSensitivityX(float Sensitivity);

	UFUNCTION(BlueprintCallable)
	void UpdateControllerSensitivityY(float Sensitivity);

	UFUNCTION(BlueprintCallable)
	void SetGamePause();

	void VibrationInLight(bool b);
	
	void CancelSkill();
	void AddMappingContext(const UInputMappingContext* MappingContext, int Priority) const;
	void RemoveMappingContext(const UInputMappingContext* MappingContext) const;
	void ToggleSkillsCooldown() const;
protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector2D ControllerDirection;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveControllerAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveKeyboardAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookControllerAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FlyAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SkillAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BiteAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LNightmareFormComboAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RNightmareFormComboAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* KeyboardNightmareFormAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractionAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* VampiricVisionAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HideAction;

private:
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem;
	
	int CounterQTE;
	bool CancelSkillBuffer;

	ABloodController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;

	void Hook();
	void MoveController(const FInputActionValue& Value);
	void MoveKeyboard(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void CheatFly(const FInputActionValue& Value);
	void Crouched();
	void StartJump();
	void StartBite();
	void EndBite();
	void StartSkill();
	void PreviewSkill();
	void InvokeSkill();
	void CheckCancelHide(const FInputActionValue& Value);
	void Interact();
	void TimingForVampiricVision();
	void SetActivVampiricVision();
	void CancelVampiricVision();
	void ReleaseCheckNighmareForm();

	void SetActiveHide();
	void CancelHide();
	
	int TriggerNightmare;
	FForceFeedbackParameters LoopingVibroParameter;
	FTimerHandle DilatationTimerHandler;
};
