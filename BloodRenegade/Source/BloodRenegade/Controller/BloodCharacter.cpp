// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodCharacter.h"

#include "AkGameplayStatics.h"
#include "BloodController.h"
#include "BloodHUD.h"
#include "Components/RectLightComponent.h"
#include "BloodPlayerState.h"
#include "HealthSystem.h"
#include "BloodRenegade/ActorComponents/BiteSystem.h"
#include "BloodRenegade/Subsystems/World/AIManagerSubsystem.h"
#include "BloodRenegade/Interfaces/IInteractable.h"
#include "BloodRenegade/AI/Soldier.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "BloodRenegade/Controller/BloodCharacterDataAsset.h"
#include "BloodRenegade/ActorComponents/HookSystem.h"
#include "BloodRenegade/ActorComponents/ClimbSystem.h"
#include "BloodRenegade/ActorComponents/HideSystem.h"
#include "BloodRenegade/ActorComponents/VampiricVisionComponent.h"
#include "BloodRenegade/DataAssets/VibationDataAsset.h"
#include "BloodRenegade/DataAssets/Sound/CharacterSoundDataAsset.h"
#include "BloodRenegade/Interfaces/DataSaver.h"
#include "Perception/AISense_Hearing.h"
#include "BloodRenegade/WorldActors/CustomSpringArm.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Curves/CurveVector.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"


// Sets default values
ABloodCharacter::ABloodCharacter()
{	
	BloodGaugeEnable = true;
	CooldownEnable = true;
	
 	PrimaryActorTick.bCanEverTick = true;
	GetCapsuleComponent()->InitCapsuleSize(24.f, 85.0f);
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600, 0.0f); // ...at this rotation rat
	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 20.f;
	GetCharacterMovement()->MaxFlySpeed = 30.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraStick = CreateDefaultSubobject<UCustomSpringArm>(TEXT("Camera Stick"));
	CameraStick->SetupAttachment(RootComponent);
	CameraStick->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraStick, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	RimLight = CreateDefaultSubobject<URectLightComponent>(TEXT("RimLight"));
	RimLight->SetupAttachment(FollowCamera);
	
	PostProcessVolume = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessVolume"));
	PostProcessVolume->SetupAttachment(CameraStick);
	
	HealthSystem = CreateDefaultSubobject<UHealthSystem>(TEXT("HEALTH"));
	BiteSystem = CreateDefaultSubobject<UBiteSystem>(TEXT("Bite"));
	HookSystemNew = CreateDefaultSubobject<UHookSystem>(TEXT("Hook System New"));
	ClimbSystem = CreateDefaultSubobject<UClimbSystem>(TEXT("Climb System"));
	HideSystem = CreateDefaultSubobject<UHideSystem>(TEXT("Hide System"));
	NewVampiricVisionComponent = CreateDefaultSubobject<UVampiricVisionComponent>(TEXT("New Vampiric Component"));

	TeamId = 1;
}

void ABloodCharacter::BeginPlay()
{
	Super::BeginPlay();
	IsDead = false;
	SpeedModifier = 1;	
	CurrentPlayerState = GetPlayerState<ABloodPlayerState>();
	CurrentPlayerState->CurrentBloodGauge = DataAsset->MaxBloodGauge;
	CurrentPlayerState->KillCounter = 0;
	ParamRaycast.AddIgnoredActor(this);
	ParamRaycast.bReturnPhysicalMaterial = true;

	CurrentPlayerState->CurrentState = NormalState;
	GetCharacterMovement()->MaxWalkSpeed = DataAsset->WalkSpeed * SpeedModifier;

	CurrentAnims = GetMesh()->GetAnimInstance();

	CurrentPlayerState->CurrentHP = DataAsset->MaxHp;
	
	GetCapsuleComponent()->GetScaledCapsuleSize(CapsuleRadius, CapsuleHeight);

	InitialFOV = FollowCamera->FieldOfView;
	CanMoveWhileHiding = true;
	CanPlayAlertSound = true;
	UpdateChangeForm(0);
	LoadPlayerSave();
	GetWorld()->GetSubsystem<UAIManagerSubsystem>()->OnAlertChangedDelegate.BindUObject(this, &ABloodCharacter::EnemiesAlertChanged);
	GetWorld()->GetSubsystem<UAIManagerSubsystem>()->OnSuspiciousChangedDelegate.BindUObject(this, &ABloodCharacter::EnemiesSuspiciousChanged);

	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::SetState(nullptr, "Music_State", "InGame");
	UAkGameplayStatics::PostEvent(SoundDataAsset->MusicAudioEvent, this, 0, NullCallback);

	StartRimLocation = RimLight->GetRelativeLocation();
	
	//-------------------------------------------------------------------------------------------
	
	FOnTimelineFloat LerpCrouchCallBack;
	LerpCrouchCallBack.BindUFunction(this, FName("CoroutineCrouch"));
	TimelineLerpCrouch.AddInterpFloat(DataAsset->CrouchCurve, LerpCrouchCallBack);

	TranslatePositionCurve = NewObject<UCurveVector>();
	FOnTimelineVector TranslateCallBack;
	FOnTimelineEventStatic TranslateCallbackFinish;
	
	TranslateCallBack.BindUFunction(this, FName("LerpPlayerPosition"));
	TranslatePlayerTimeline.AddInterpVector(TranslatePositionCurve, TranslateCallBack);

	TranslateCallbackFinish.BindUFunction(this, FName("FinishTranslate"));
	TranslatePlayerTimeline.SetTimelineFinishedFunc(TranslateCallbackFinish);

	ModifFovCurv = NewObject<UCurveFloat>();
	ModifFovCurv->FloatCurve.AddKey(0,0);
	ModifFovCurv->FloatCurve.AddKey(0.3f,1);

	FOnTimelineFloat FovCallBack;
	FovCallBack.BindUFunction(this, FName("LerpFOVCamera"));
	ModifFovTimeline.AddInterpFloat(ModifFovCurv, FovCallBack);
}

void ABloodCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	EnableInput(GetWorld()->GetFirstPlayerController());
}

// Called every frame
void ABloodCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float f = FMath::GetMappedRangeValueClamped(FVector2d(170,370), FVector2d(0,200), CameraStick->TargetArmLength);
	RimLight->SetRelativeLocation(StartRimLocation + RimLight->GetRelativeLocation().ForwardVector*f);
	
	TimelineLerpCrouch.TickTimeline(DeltaTime);
	TranslatePlayerTimeline.TickTimeline(DeltaTime);
	ModifFovTimeline.TickTimeline(DeltaTime);
	
	bool IsGround = IsGrounded();
	
	if(CurrentPlayerState->ObjectInteractable.Get())
	{
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetController<ABloodController>(),CurrentPlayerState->ObjectInteractable->GetActorLocation() + (FVector::UpVector * IIInteractable::Execute_GetOffset(CurrentPlayerState->ObjectInteractable.Get())),InteractScreenLoc,true);
		GetController<ABloodController>()->CurrentHUD->SetInteractInputPos(InteractScreenLoc);
	}
	
	if (GetCharacterMovement()->bCheatFlying)
	{
		return;
	}

	if(WasGrounded != IsGround)
	{
		if(IsGround)
		{
			IsCoyoteTime = false;
			GetWorldTimerManager().ClearTimer(CoyoteTimerHandle);
			BiteSystem->SetCurrentBiteType(OnGround);
			
			if(!HookSystemNew->IsHooking && !CurrentPlayerState->IsDashing)
			{
				CameraStick->CameraLagSpeed = 20;
			}
			
			SetCrouch(WasCrouched);
			
			ClimbSystem->TimelineCheckLedge.Stop();

			if(!IsDead && FallVelocity < -DataAsset ->FallVelocityForRecover && !BiteSystem->IsBiting)
			{
				StartJumpRecovery();
			}
		}else
		{
			if(GetCharacterMovement()->Velocity.Z<=0)
			{				
				TWeakObjectPtr<ABloodCharacter> WeakThis = this;
				IsCoyoteTime = true;
				GetWorldTimerManager().SetTimer(CoyoteTimerHandle, FTimerDelegate::CreateWeakLambda(this, 
					   [WeakThis]
					   {
						   if(WeakThis.IsValid())
						   {
								WeakThis->IsCoyoteTime = false;
						   }
					   }), 0.3f, false);

				WasCrouched = CurrentPlayerState->IsCrouched;
			}
			
			if(!BiteSystem->IsTraveling)
			{
				BiteSystem->SetCurrentBiteType(InAir);
				ClimbSystem->TimelineCheckLedge.PlayFromStart();
			}
		}
	}

	if(!IsGround)
	{
		FallVelocity = GetCharacterMovement()->Velocity.Z;
	}
	
	WasGrounded = IsGround;
}

void ABloodCharacter::StartTranslateCharacter(FVector start, FVector destination, float timing)
{
	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->HookFadeInAudioEvent, this, 0, NullCallback);
	LastTiming = timing;
	EffectBeforeHook();
	CurrentPlayerState->CurrentState = CantMoveAndRotate;
	GetCharacterMovement()->StopActiveMovement();
	FRotator pivot = (destination-start).Rotation();
	pivot.Pitch = 0;
	SetActorRotation(pivot);
	
	TranslatePositionCurve->FloatCurves[0].Reset();
	TranslatePositionCurve->FloatCurves[1].Reset();
	TranslatePositionCurve->FloatCurves[2].Reset();
	
	TranslatePositionCurve->FloatCurves[0].AddKey(0, start.X);
	TranslatePositionCurve->FloatCurves[1].AddKey(0,start.Y);
	TranslatePositionCurve->FloatCurves[2].AddKey(0,start.Z);

	TranslatePositionCurve->FloatCurves[0].AddKey(timing, destination.X);
	TranslatePositionCurve->FloatCurves[1].AddKey(timing,destination.Y);
	TranslatePositionCurve->FloatCurves[2].AddKey(timing,destination.Z);

	TranslatePlayerTimeline.PlayFromStart();
}

void ABloodCharacter::SetMaxFOV(float f)
{
	MaxFOV = f;
}

void ABloodCharacter::EnemiesAlertChanged(bool IsAlerted)
{
	if (IsAlerted)
	{
		IsSeenBySoldier = true;
		UAkGameplayStatics::SetState(nullptr, "Music_State", "Alerted");

		if (CanPlayAlertSound)
		{
			CanPlayAlertSound = false;
			
			const FOnAkPostEventCallback NullCallback;
			UAkGameplayStatics::PostEvent(SoundDataAsset->EnterAlertStateEvent, this, 0, NullCallback);
		}
	}
	else
	{
		IsSeenBySoldier = false;
		UAkGameplayStatics::SetState(nullptr, "Music_State", "InGame");
		HealthSystem->ResetHP();
	}
}

void ABloodCharacter::EnemiesSuspiciousChanged(bool IsSuspicious)
{
	if (IsSuspicious)
	{
		const FOnAkPostEventCallback NullCallback;
		UAkGameplayStatics::PostEvent(SoundDataAsset->EnterSuspiciousStateEvent, this, 0, NullCallback);
	}
	else
	{
		CanPlayAlertSound = true;
	}
}

void ABloodCharacter::LerpFOVCamera(float Value)
{
	FollowCamera->FieldOfView = FMath::Lerp(InitialFOV,MaxFOV,Value);
}

void ABloodCharacter::LerpPlayerPosition(FVector value)
{
	float percent = TranslatePlayerTimeline.GetPlaybackPosition()/LastTiming;
	float currentModif = DataAsset->TeleportAccelerationCurve->GetFloatValue(percent);
	value.Z = value.Z*currentModif;
	SetActorLocation(value);
	
	if(FMath::FloorToInt(percent*100) >= 50)
	{
		if(!HookSystemNew->ResetCamDone)
		{
			HookSystemNew->ResetCamHook();
		}
	}
}

void ABloodCharacter::FinishTranslate()
{
	CurrentPlayerState->CurrentState = NormalState;
	GetCharacterMovement()->StopActiveMovement();
	SetBlendMask(false);
	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->HookFadeOutAudioEvent, this, 0, NullCallback);

	EffectAfterHook();
	
	if(CurrentPlayerState->IsHiding)
	{
		HideSystem->CancelHide();
	}

	if(HookSystemNew->IsHooking)
	{
		HookSystemNew->FinishHook();
	}

	if(BiteSystem->IsTraveling)
	{
		BiteSystem->StartBiting();
		return;
	}

	PlayAnimMontage(HookMontage);
}

void ABloodCharacter::StartJumpRecovery()
{
	CurrentPlayerState->CurrentState = CantMove;
	GetController<ABloodController>()->CancelSkill();
	PlayAnimMontage(HardJumpRecoveryMontage);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1, this, DataAsset->FallSoundRange, FName(TEXT("Instant")));
	GetController<ABloodController>()->ClientPlayForceFeedback(VibrationDataAsset->LoudLand);
	GetWorldTimerManager().SetTimer(JumpRecoveryTimerHandle, this, &ABloodCharacter::EndJumpRecovery, DataAsset->TimeForRecover, false);
	GetCharacterMovement()->MaxAcceleration = 0;
}

void ABloodCharacter::EndJumpRecovery()
{
	CurrentPlayerState->CurrentState = NormalState;
	GetCharacterMovement()->MaxAcceleration = 1500;
}

void ABloodCharacter::Interact()
{
	PlayAnimMontage(InteractAnimMontage);
	IIInteractable::Execute_Interact(CurrentPlayerState->ObjectInteractable.Get());
	
	if (!CurrentPlayerState->IsEscorting)
	{
		UnsetInteractable(CurrentPlayerState->ObjectInteractable.Get());
	}
}

void ABloodCharacter::Stunned(float f)
{
	if (BiteSystem->IsBiting) return;
	
	bool b = true;
	GetController<ABloodController>()->PlayerCameraManager->StartCameraShake(DataAsset->CameraShakeStrike, 1);
	CurrentPlayerState->IsStunned = true;
	CurrentPlayerState->CurrentState = CantMove;
	CurrentAnims->ProcessEvent(CurrentAnims->FindFunction(FName("SetIsStun")), &b);

	GetWorldTimerManager().SetTimer(StrikeTimerHandle, this, &ABloodCharacter::UnStunned, f, false); 
}

void ABloodCharacter::EnableCanBeStrike()
{
	CanBeStrike.Execute(true);
}

void ABloodCharacter::UnStunned()
{
	bool b = false;
	CurrentAnims->ProcessEvent(CurrentAnims->FindFunction(FName("SetIsStun")), &b);
	CurrentPlayerState->IsStunned = false;
	CurrentPlayerState->CurrentState = NormalState;
	GetWorldTimerManager().SetTimer(StrikeRecoveryTimerHandle, this, &ABloodCharacter::EnableCanBeStrike, DataAsset->StrikeRecoveryTimer, false); 
}

void ABloodCharacter::Move(FVector2D _dir)
{
	if (Controller != nullptr)
	{
		
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		
		FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

#if !UE_BUILD_SHIPPING
		if (GetCharacterMovement()->bCheatFlying)
		{
			GetCharacterMovement()->Velocity = (ForwardDirection * _dir.Y + RightDirection * _dir.X) * GetCharacterMovement()->MaxFlySpeed / GetWorld()->GetDeltaSeconds();
			AddMovementInput(ForwardDirection, 0.01f);
			AddMovementInput(RightDirection, 0.01f);
			return;
		}
#endif
		
		if (!CurrentPlayerState->IsCrouched)
		{
			UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1, this, DataAsset->StepSoundRange, FName(TEXT("Incremental")));
		}

		if(!CurrentPlayerState->IsHiding)
		{
			AddMovementInput(ForwardDirection, _dir.Y);
			AddMovementInput(RightDirection, _dir.X);
			return;
		}
		
		float Test1 = round(ForwardDirection.Dot(GetActorRightVector()));
		float Test2 = round(RightDirection.Dot(GetActorRightVector()));
		
		if(CanMoveWhileHiding)
		{
			AddMovementInput(GetActorRightVector()* Test1, _dir.Y);
			AddMovementInput(GetActorRightVector() * Test2, _dir.X);
		}

		XDirection = FMath::Sign((Test1*_dir.Y + Test2*_dir.X)/2);
	}
}

void ABloodCharacter::Look(FVector2D look)
{
	AddControllerYawInput(look.X);
	AddControllerPitchInput(look.Y);
	
	
	if(GetController<ABloodController>()->PlayerCameraManager->GetCameraRotation().Pitch<=-60 && WasGrounded)
	{
		BiteSystem->SetCurrentBiteType(OnLedge);
	}else
	{
		if(WasGrounded)
		{
			BiteSystem->SetCurrentBiteType(OnGround);
		}else
		{
			BiteSystem->SetCurrentBiteType(InAir);
		}
	}
}

void ABloodCharacter::Fly(const float Dir)
{
	SetActorLocation(GetActorLocation() + FVector::UpVector * Dir * 10 * GetWorld()->GetDeltaSeconds());
}

void ABloodCharacter::SetCrouch(bool b)
{
	if(!IsGrounded() || b == CurrentPlayerState->IsCrouched) return;
	
	if(CurrentPlayerState->IsCrouched)
	{
		FHitResult RaycastResult1;
		FCollisionShape Shape = FCollisionShape::MakeBox(FVector(15,15,70));
		GetWorld()->SweepSingleByChannel(RaycastResult1, GetActorLocation()+GetActorUpVector()*CapsuleHeight, GetActorLocation()+GetActorUpVector()*CapsuleHeight, GetActorRotation().Quaternion(), ECC_WorldDynamic, Shape, ParamRaycast);

		//FBox test = FBox(FVector(-20,-20,-70), FVector(20,20,70));
		//DrawDebugSolidBox(GetWorld(), test, FColor::Blue, FTransform(GetActorRotation(), GetActorLocation()+GetActorUpVector()*CapsuleHeight), true, 0.1f);
		
		if(RaycastResult1.bBlockingHit) return;
	}
	
	CurrentPlayerState->IsCrouched = b;

	if(b)
	{
		TimelineLerpCrouch.Play();
		return;
	}
	
	TimelineLerpCrouch.Reverse();
}

void ABloodCharacter::StartNightmareForm(int numberEnemy)
{
	SetCrouch(false);
	BiteSystem->SetCurrentBiteType(NightmareForm);
	EffectStartNightmareForm();
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
	CustomTimeDilation = 10.0f;
	BiteSystem->EnemiEndAlive = numberEnemy;
	NewVampiricVisionComponent->StartVision();
}

void ABloodCharacter::SetActiveNightmareForm()
{
	CurrentPlayerState->IsInNightmareForm = true;
}

void ABloodCharacter::CheckEndGame(int i)
{
	if(i<=0)
	{
		NewVampiricVisionComponent->StopVision();
		StartEndGame();
	}
}

void ABloodCharacter::CoroutineCrouch(float Value)
{
	GetCapsuleComponent()->SetCapsuleSize(24,FMath::Lerp(90,50, Value));
	GetMesh()->SetRelativeLocation(FVector(0,0,FMath::Lerp(-90,-50, Value)));
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(DataAsset->WalkSpeed,
		DataAsset->CrouchSpeed, Value) * SpeedModifier;
	GetCapsuleComponent()->GetScaledCapsuleSize(CapsuleRadius, CapsuleHeight);
}

FGenericTeamId ABloodCharacter::GetGenericTeamId() const 
{
	return TeamId;
}

bool ABloodCharacter::IsGrounded()
{
	if(HookSystemNew->IsHooking ) return false;
	
	FHitResult LocalResult;
	FCollisionShape Shape = FCollisionShape::MakeBox(FVector(15,15,10));
	GetWorld()->SweepSingleByChannel(LocalResult, GetActorLocation()-GetActorUpVector()*CapsuleHeight, GetActorLocation()-GetActorUpVector()*CapsuleHeight, GetActorRotation().Quaternion(), ECC_WorldDynamic, Shape, ParamRaycast);

	//FBox test = FBox(FVector(-15,-15,-10), FVector(15,15,10));
	//DrawDebugSolidBox(GetWorld(), test, FColor::Blue, FTransform(GetActorRotation(), GetActorLocation()-GetActorUpVector()*CapsuleHeight), false, 0.1f);
	
	if(LocalResult.bBlockingHit)
	{
		if(LocalResult.GetComponent()->ComponentTags.Contains("Ledge"))
		{
			if(!CurrentPlayerState->IsOnLedge)
			{
				CurrentPlayerState->IsOnLedge = true;
				WasCrouched = true;
			}
		}else
		{
			if(CurrentPlayerState->IsOnLedge)
			{
				CurrentPlayerState->IsOnLedge = false;
			}
		}

		if (LocalResult.PhysMaterial.IsValid())
		{
			CurrentSurfaceType = LocalResult.PhysMaterial->SurfaceType;
		}
		
		return true;
	}

	if(CurrentPlayerState->IsOnLedge)
	{
		CurrentPlayerState->IsOnLedge = false;
	}
	
	return false;
}

void ABloodCharacter::ActivateSkill()
{
	CurrentPlayerState->IsAiming = true;
	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->BlindAimLoop, this, 0, NullCallback);
	
	CurrentAnims->ProcessEvent(CurrentAnims->FindFunction(FName("ActiveSkill")), NULL);
	bUseControllerRotationYaw = true;
	CameraStick->SetCameraArmDist(170);
	if(!CurrentPlayerState->IsHiding)
	{
		CameraStick->EnableAimCamPosition(1);
	}
}

void ABloodCharacter::CancelSkill()
{
	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->BlindAimLoopStop, this, 0, NullCallback);
	
	CurrentPlayerState->IsAiming = false;
	CurrentAnims->ProcessEvent(CurrentAnims->FindFunction(FName("ReleaseSkill")), NULL);
	bUseControllerRotationYaw = false;
	CameraStick->DisableAimCamPosition();
}

void ABloodCharacter::TakeHit(int dmg)
{
	if (!CanBeDamaged() || CurrentPlayerState->IsInNightmareForm) return;
	SetBlendMask(true);

	if(!BiteSystem->IsBiting)
	{
		PlayAnimMontage(HitMontage);
	}
	
	GetController<ABloodController>()->PlayerCameraManager->StartCameraShake(DataAsset->CameraShakeTakeDamage, 1);
	GetController<ABloodController>()->ClientPlayForceFeedback(VibrationDataAsset->TakeItEffect);
	HealthSystem->TakeHit(dmg);

	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->TakeDamageEvent, this, 0, NullCallback);
	TakeHitEffect(true);
	SetCrouch(false);
}

void ABloodCharacter::SetCantMove(bool b)
{
	if(b)
	{
		CurrentPlayerState->CurrentState = CantMoveAndRotate;
		return;
	}

	CurrentPlayerState->CurrentState = NormalState;
}

void ABloodCharacter::SetActiveEscort(bool b)
{
	CurrentPlayerState->IsEscorting = b;
	bool pivot;
	if(b)
	{
		SetCrouch(false);
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 200, 0.0f); // ...
		GetCapsuleComponent()->SetCapsuleRadius(90);
		GetCharacterMovement()->MaxWalkSpeed = GetCurrentStateSpeed();
		CurrentPlayerState->CurrentState = CanOnlyMove;
		pivot = true;
	}else
	{
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 600, 0.0f); // ...
		GetCapsuleComponent()->SetCapsuleRadius(CapsuleRadius);
		GetCharacterMovement()->MaxWalkSpeed = GetCurrentStateSpeed();
		CurrentPlayerState->CurrentState = NormalState;
		pivot = false;
	}

	CurrentAnims->ProcessEvent(CurrentAnims->FindFunction(FName("SetEscort")), &pivot);
}

bool ABloodCharacter::GetIsSeenBySoldier()
{
	return IsSeenBySoldier;
}


void ABloodCharacter::SetBloodGauge(const int GaugeValue)
{
	if (!BloodGaugeEnable)
		return;
	
	CurrentPlayerState->CurrentBloodGauge = FMath::Clamp(GaugeValue, 0, DataAsset->MaxBloodGauge);

	GetController<ABloodController>()->GetHUD<ABloodHUD>()->UpdateAllBlood(CurrentPlayerState->CurrentBloodGauge);
}

void ABloodCharacter::ClientCheatWalk_Implementation()
{
#if !UE_BUILD_SHIPPING
	SetActorEnableCollision(true);

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->bCheatFlying = false;
		MovementComponent->SetMovementMode(MOVE_Falling);
	}
#endif
}

void ABloodCharacter::ClientCheatGhost_Implementation()
{
#if !UE_BUILD_SHIPPING
	SetActorEnableCollision(false);
	ClimbSystem->TimelineCheckLedge.Stop();
	WasGrounded = false;
	
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->bCheatFlying = true;
		MovementComponent->SetMovementMode(MOVE_Flying);
	}
#endif
}

float ABloodCharacter::GetCurrentStateSpeed() const
{
	if (CurrentPlayerState->IsCrouched)
	{
		return DataAsset->CrouchSpeed;
	}

	if(CurrentPlayerState->IsEscorting)
	{
		return DataAsset->EscortSpeed;
	}
	
	return DataAsset->WalkSpeed;
}

void ABloodCharacter::SetModifier(float f)
{
	SpeedModifier = f;
	GetCharacterMovement()->MaxWalkSpeed = GetCurrentStateSpeed() * SpeedModifier;
}

void ABloodCharacter::Death()
{
	EffectDeath();
	IsDead = true;
	GetController<ABloodController>()->bForceFeedbackEnabled = false;
	GetController()->DisableInput(GetController<APlayerController>());
	IDataSaver::Execute_LoadGameData(GetGameInstance());
}

void ABloodCharacter::Jump()
{
	if((!WasGrounded && !IsCoyoteTime) || CurrentPlayerState->IsHiding) return;

	if(!ClimbSystem->CheckVault())
	{
		WasCrouched = CurrentPlayerState->IsCrouched;
		SetCrouch(false);
		GetWorldTimerManager().ClearTimer(CoyoteTimerHandle);
		if(IsCoyoteTime)
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
		Super::Jump();
		ClimbSystem->TimelineCheckLedge.PlayFromStart();
		IsCoyoteTime = false;
		const FOnAkPostEventCallback NullCallback;
		UAkGameplayStatics::PostEvent(SoundDataAsset->JumpLeadEvent, this, 0, NullCallback);
	}
}


UStaticMeshComponent* ABloodCharacter::GetVampiricSphereMesh()
{
	return NewVampiricVisionComponent->SphereVisionVampiric->GetStaticMeshComponent();
}

void ABloodCharacter::PlayFootstepSound()
{
	if (CurrentSurfaceType == SurfaceType_Default)
		return;
	
	const UAkSwitchValue* AkSwitchValue = SoundDataAsset->FootstepsSwitchValues[CurrentSurfaceType];
	UAkGameplayStatics::SetSwitch(AkSwitchValue, this, "", "");
	
	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->FootstepsAudioEvent, this, 0, NullCallback);
}

void ABloodCharacter::SetCurrentInteractable(AActor* InteractableActor)
{
	if(!CurrentPlayerState || CurrentPlayerState->IsEscorting) return;
	
	CurrentPlayerState->ObjectInteractable = InteractableActor;
	GetController<ABloodController>()->CurrentHUD->SetActiveInteractInput(true);
}

void ABloodCharacter::UnsetInteractable(AActor* InteractableActor)
{
	if(!CurrentPlayerState) return;
	
	if(CurrentPlayerState->ObjectInteractable.Get() != InteractableActor) return;
	
	if (GetController<ABloodController>()->CurrentHUD.IsValid())
	{
		GetController<ABloodController>()->CurrentHUD->SetActiveInteractInput(false);
	}
		
	CurrentPlayerState->ObjectInteractable = nullptr;
}
