// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodController.h"

#include "AkGameplayStatics.h"
#include "AkGameplayTypes.h"
#include "BloodPlayerState.h"
#include "BloodRenegade/Skills/Blind.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BloodRenegade/Tools/BloodCheatManager.h"
#include "BloodCharacter.h"
#include "BloodRenegade/DataAssets/Sound/CharacterSoundDataAsset.h"
#include "BloodCharacterDataAsset.h"
#include "BloodRenegade/ActorComponents/BiteSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BloodRenegade/Controller/BloodHUD.h"
#include "GameFramework/Character.h"
#include "BloodRenegade/ActorComponents/HookSystem.h"
#include "BloodRenegade/ActorComponents/HideSystem.h"
#include "BloodRenegade/ActorComponents/VampiricVisionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BloodRenegade/DataAssets/VibationDataAsset.h"
#include "BloodRenegade/Interfaces/IBlindable.h"
#include "BloodRenegade/Settings/BloodGameUserSettings.h"
#include "Kismet/KismetMathLibrary.h"

ABloodController::ABloodController()
{
	CheatClass = UBloodCheatManager::StaticClass();

	BlindSkill = CreateDefaultSubobject<UBlind>(TEXT("Blind Skill"));
	AddOwnedComponent(BlindSkill);
	BlindSkill->PrimaryComponentTick.bCanEverTick = true;
	BlindSkill->SetComponentTickEnabled(true);
}

void ABloodController::BeginPlay()
{
	Super::BeginPlay();
	CurrentPlayerState = GetPlayerState<ABloodPlayerState>();
	CurrentHUD = GetHUD<ABloodHUD>();
	PossesedPawn = Cast<ABloodCharacter>(GetCharacter());
	BlindSkill->HUD = CurrentHUD;
	
	Possess(PossesedPawn.Get());
	
	PlayerCameraManager->ViewPitchMax = 50;
	PlayerCameraManager->ViewPitchMin = -80;
	
	LoopingVibroParameter.bIgnoreTimeDilation = false;
	LoopingVibroParameter.bLooping = true;
	bForceFeedbackEnabled = UBloodGameUserSettings::GetBloodGameUserSettings()->ControllerVibrationEnabled;
}

void ABloodController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	
	if(InputSubsystem)
	{
		InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveControllerAction, ETriggerEvent::Triggered, this, &ABloodController::MoveController);
		EnhancedInputComponent->BindAction(MoveControllerAction, ETriggerEvent::Started, this, &ABloodController::CheckCancelHide);
		EnhancedInputComponent->BindAction(MoveControllerAction, ETriggerEvent::Completed, this, &ABloodController::TimingForVampiricVision);
		EnhancedInputComponent->BindAction(MoveControllerAction, ETriggerEvent::Started, this, &ABloodController::CancelVampiricVision);

		EnhancedInputComponent->BindAction(MoveKeyboardAction, ETriggerEvent::Triggered, this, &ABloodController::MoveKeyboard);
		EnhancedInputComponent->BindAction(MoveKeyboardAction, ETriggerEvent::Started, this, &ABloodController::CheckCancelHide);
		EnhancedInputComponent->BindAction(MoveKeyboardAction, ETriggerEvent::Completed, this, &ABloodController::TimingForVampiricVision);
		EnhancedInputComponent->BindAction(MoveKeyboardAction, ETriggerEvent::Started, this, &ABloodController::CancelVampiricVision);

		EnhancedInputComponent->BindAction(LookMouseAction, ETriggerEvent::Triggered, this, &ABloodController::Look);
		EnhancedInputComponent->BindAction(LookControllerAction, ETriggerEvent::Triggered, this, &ABloodController::Look);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ABloodController::Crouched);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &ABloodController::SetGamePause);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ABloodController::StartJump);

		EnhancedInputComponent->BindAction(HookAction, ETriggerEvent::Started, this, &ABloodController::Hook);
		EnhancedInputComponent->BindAction(InteractionAction, ETriggerEvent::Started, this, &ABloodController::Interact);
		
		EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Started, this, &ABloodController::StartSkill);
		EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Triggered, this, &ABloodController::PreviewSkill);
		EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Completed, this, &ABloodController::InvokeSkill);

		EnhancedInputComponent->BindAction(BiteAction, ETriggerEvent::Started, this, &ABloodController::StartBite);
		EnhancedInputComponent->BindAction(BiteAction, ETriggerEvent::Started, this, &ABloodController::CancelSkill);
		EnhancedInputComponent->BindAction(BiteAction, ETriggerEvent::Completed, this, &ABloodController::EndBite);

		EnhancedInputComponent->BindAction(VampiricVisionAction, ETriggerEvent::Started, this, &ABloodController::SetActivVampiricVision);
		EnhancedInputComponent->BindAction(HideAction, ETriggerEvent::Started, this, &ABloodController::SetActiveHide);
		EnhancedInputComponent->BindAction(HideAction, ETriggerEvent::Completed, this, &ABloodController::CancelHide);

#if UE_WITH_CHEAT_MANAGER
		EnhancedInputComponent->BindAction(FlyAction, ETriggerEvent::Triggered, this, &ABloodController::CheatFly);
#endif
	}
}

void ABloodController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(BlindSkill->LastTarget.Get())
	{
		FRotator rotateTo = UKismetMathLibrary::FindLookAtRotation(PlayerCameraManager->GetCameraLocation(), BlindSkill->LastTarget->GetActorLocation()+ FVector::UpVector*IIBlindable::Execute_GetBlindOffset(BlindSkill->LastTarget.Get()));
		SetControlRotation(rotateTo);
	}
}

void ABloodController::Hook()
{
	if (!CurrentPlayerState->HookTarget.Get() || CurrentPlayerState->IsInNightmareForm || CurrentPlayerState->CurrentState != NormalState) return;
	PossesedPawn->NewVampiricVisionComponent->StopVision();
	PossesedPawn->HookSystemNew->Hook();
}

void ABloodController::MoveController(const FInputActionValue& value)
{
	if(CurrentPlayerState->CurrentState != NormalState && CurrentPlayerState->CurrentState != CanOnlyMove) return;
	
	PossesedPawn->Move(value.Get<FVector2D>());
}

void ABloodController::MoveKeyboard(const FInputActionValue& Value)
{
	if(CurrentPlayerState->CurrentState != NormalState && CurrentPlayerState->CurrentState != CanOnlyMove) return;
	
	PossesedPawn->Move(Value.Get<FVector2D>());
}

void ABloodController::Look(const FInputActionValue& value)
{
	if(CurrentPlayerState->CurrentState == CantMoveAndRotate) return;
	
	PossesedPawn->Look(value.Get<FVector2D>());
}

void ABloodController::Crouched()
{
	if (CurrentPlayerState->IsInNightmareForm || CurrentPlayerState->CurrentState != NormalState) return;
	
	PossesedPawn->SetCrouch(!CurrentPlayerState->IsCrouched);
}

void ABloodController::StartSkill()
{
	GetWorld()->GetTimerManager().ClearTimer(DilatationTimerHandler);
	if(CurrentPlayerState->CurrentState != NormalState || CurrentPlayerState->IsInNightmareForm || (CurrentPlayerState->IsHiding && PossesedPawn->CanMoveWhileHiding))
	{
		CancelSkill();
		return;
	}
	
	PossesedPawn->NewVampiricVisionComponent->StopVision();
	ClientPlayForceFeedback(VibrationDataAsset->StartSpell, LoopingVibroParameter);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.3f);
	TWeakObjectPtr<ABloodController> WeakThis = this;
	GetWorld()->GetTimerManager().SetTimer(DilatationTimerHandler, FTimerDelegate::CreateWeakLambda(this, 
					   [WeakThis]
					   {
						   if(WeakThis.IsValid())
						   {
						   		UGameplayStatics::SetGlobalTimeDilation(WeakThis->PossesedPawn->GetWorld(), 1);
						   }
					   }), DataAsset->TimerDilatationTime*0.3f, false);
	
	CancelSkillBuffer = false;
	PossesedPawn->ActivateSkill();
	BlindSkill->Start();
}

void ABloodController::PreviewSkill()
{
	if(CurrentPlayerState->CurrentState != NormalState || CancelSkillBuffer || (CurrentPlayerState->IsHiding && PossesedPawn->CanMoveWhileHiding))
	{
		CancelSkill();
		return;
	}
	
	BlindSkill->PreviewSkillTarget();
}


void ABloodController::InvokeSkill()
{
	if(CurrentPlayerState->CurrentState != NormalState || CurrentPlayerState->IsInNightmareForm || CancelSkillBuffer || (CurrentPlayerState->IsHiding && PossesedPawn->CanMoveWhileHiding))
	{
		CancelSkill();
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(DilatationTimerHandler);
	CurrentPlayerState->IsAiming = false;
	PossesedPawn->CancelSkill();
	BlindSkill->Invoke();
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	ClientStopForceFeedback(VibrationDataAsset->StartSpell, FName("None"));
}

void ABloodController::CancelSkill()
{
	if(!CurrentPlayerState->IsAiming) return;
	GetWorld()->GetTimerManager().ClearTimer(DilatationTimerHandler);
	CurrentPlayerState->IsAiming = false;
	PossesedPawn->CancelSkill();
	PossesedPawn->SetBlendMask(false);
	BlindSkill->Cancel();
	CancelSkillBuffer = true;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	ClientStopForceFeedback(VibrationDataAsset->StartSpell, FName("None"));
}

void ABloodController::CheckCancelHide(const FInputActionValue& Value)
{
	if(!CurrentPlayerState->IsHiding || PossesedPawn->CanMoveWhileHiding) return;

	FVector2D NormalizedDir = Value.Get<FVector2D>();
	NormalizedDir.Normalize();
	//PossesedPawn->HideSystem->CheckCancelHide(NormalizedDir);
}

void ABloodController::Interact()
{
	if(!CurrentPlayerState->ObjectInteractable.Get()) return;
	PossesedPawn->NewVampiricVisionComponent->StopVision();
	PossesedPawn->Interact();
}

void ABloodController::TimingForVampiricVision()
{
	CurrentHUD->SetActiveInputList(true);
	PossesedPawn->NewVampiricVisionComponent->CanUseVV = true;
	PossesedPawn->XDirection = 0;
}

void ABloodController::SetActivVampiricVision()
{
	PossesedPawn->NewVampiricVisionComponent->SwitchVision();
}

void ABloodController::CancelVampiricVision()
{
	if(!CurrentPlayerState->IsInNightmareForm)
	{
		CurrentHUD->SetActiveInputList(false);
		PossesedPawn->NewVampiricVisionComponent->CanUseVV = false;
		PossesedPawn->NewVampiricVisionComponent->StopVision();
	}
}

void ABloodController::SetGamePause()
{
	CurrentHUD->SetActivePauseMenu(!IsPaused());
	bShowMouseCursor = !IsPaused();
	bEnableClickEvents = !IsPaused();
	
	if(IsPaused())
	{
		SetPause(false);
		SetInputMode(FInputModeGameOnly());
		return;
	}
	
	SetPause(true);
}

void ABloodController::VibrationInLight(bool b)
{
	if(b)
	{
		ClientPlayForceFeedback(VibrationDataAsset->InLight, LoopingVibroParameter);
	}else
	{
		ClientStopForceFeedback(VibrationDataAsset->InLight, FName(""));
	}
}

void ABloodController::StartJump()
{
	if(CurrentPlayerState->CurrentState != NormalState) return;
	if(!CurrentPlayerState->IsInNightmareForm)
	{
		PossesedPawn->NewVampiricVisionComponent->StopVision();
	}
	PossesedPawn->Jump();
}

void ABloodController::StartBite()
{
	if(!CurrentPlayerState->IsInNightmareForm)
	{
		PossesedPawn->NewVampiricVisionComponent->StopVision();
	}
	PossesedPawn->BiteSystem->CheckInputQTE();
}

void ABloodController::EndBite()
{
	PossesedPawn->BiteSystem->EndBite();
}

void ABloodController::ReleaseCheckNighmareForm()
{
	UE_LOG(LogTemp,Error,TEXT("--"));
	TriggerNightmare--;

	if(TriggerNightmare<0)
	{
		TriggerNightmare=0;
	}
}

void ABloodController::SetActiveHide()
{
	PossesedPawn->HideSystem->CheckHide();
}

void ABloodController::CancelHide()
{
	PossesedPawn->HideSystem->CancelHide();
}

void ABloodController::CheatFly(const FInputActionValue& Value)
{
	#if UE_WITH_CHEAT_MANAGER
		if(CurrentPlayerState->CurrentState != NormalState || !PossesedPawn->GetCharacterMovement()->bCheatFlying) return;

		PossesedPawn->Fly(Value.Get<float>() * 100);
	#endif
}

void ABloodController::AddMappingContext(const UInputMappingContext* MappingContext, const int Priority) const
{
	InputSubsystem->AddMappingContext(MappingContext, Priority);
}

void ABloodController::RemoveMappingContext(const UInputMappingContext* MappingContext) const
{
	InputSubsystem->RemoveMappingContext(MappingContext);
}

void ABloodController::ToggleSkillsCooldown() const
{
	#if UE_WITH_CHEAT_MANAGER
		PossesedPawn->CooldownEnable = !PossesedPawn->CooldownEnable;
	#endif
}

void ABloodController::UpdateMouseSensitivityX(float Sensitivity)
{
	const FInputActionInstance* InputActionInstance = InputSubsystem->GetPlayerInput()->FindActionInstanceData(LookMouseAction);
	UInputModifierScalar* InputModifierScalar = Cast<UInputModifierScalar>(InputActionInstance->GetModifiers()[0]);
	InputModifierScalar->Scalar.X = Sensitivity;
}

void ABloodController::UpdateMouseSensitivityY(float Sensitivity)
{
	const FInputActionInstance* InputActionInstance = InputSubsystem->GetPlayerInput()->FindActionInstanceData(LookMouseAction);
	UInputModifierScalar* InputModifierScalar = Cast<UInputModifierScalar>(InputActionInstance->GetModifiers()[0]);
	InputModifierScalar->Scalar.Y = Sensitivity;
}

void ABloodController::UpdateControllerSensitivityX(float Sensitivity)
{
	const FInputActionInstance* InputActionInstance = InputSubsystem->GetPlayerInput()->FindActionInstanceData(LookControllerAction);
	UInputModifierScalar* InputModifierScalar = Cast<UInputModifierScalar>(InputActionInstance->GetModifiers()[0]);
	InputModifierScalar->Scalar.X = Sensitivity;
}

void ABloodController::UpdateControllerSensitivityY(float Sensitivity)
{
	const FInputActionInstance* InputActionInstance = InputSubsystem->GetPlayerInput()->FindActionInstanceData(LookControllerAction);
	UInputModifierScalar* InputModifierScalar = Cast<UInputModifierScalar>(InputActionInstance->GetModifiers()[0]);
	InputModifierScalar->Scalar.Y = Sensitivity;
}
