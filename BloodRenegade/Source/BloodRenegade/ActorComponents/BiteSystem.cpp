// Fill out your copyright notice in the Description page of Project Settings.


#include "BiteSystem.h"
#include "HideSystem.h"
#include "AkGameplayStatics.h"
#include "BloodRenegade/AI/Soldier.h"
#include "BloodRenegade/Controller/BloodCharacter.h"
#include "BloodRenegade/Controller/BloodCharacterDataAsset.h"
#include "BloodRenegade/Controller/BloodController.h"
#include "BloodRenegade/Interfaces/IBitable.h"
#include "BloodRenegade/Controller/BloodPlayerState.h"
#include "BloodRenegade/Controller/BloodHUD.h"
#include "BloodRenegade/DataAssets/VibationDataAsset.h"
#include "BloodRenegade/Skills/Blind.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UBiteSystem::UBiteSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBiteSystem::BeginPlay()
{
	Super::BeginPlay();

	ActorOwner = GetOwner<ABloodCharacter>();
	CurrentPlayerState =ActorOwner->GetPlayerState<ABloodPlayerState>();
	ParamRaycast.AddIgnoredActor(ActorOwner.Get());
	CurrentHUD = ActorOwner->GetController<ABloodController>()->GetHUD<ABloodHUD>();
	PlayerCameraManager = ActorOwner->GetController<ABloodController>()->PlayerCameraManager;
	CurrentController = ActorOwner->GetController<ABloodController>();
	CurrentBiteType = OnGround;

	UCurveFloat* CurveQTE = NewObject<UCurveFloat>();
	CurveQTE->FloatCurve.AddKey(0,0);
	CurveQTE->FloatCurve.AddKey(DataAsset->DurationQTE, 1);

	FOnTimelineFloat QTETimingCallback;
	FOnTimelineEventStatic QTETimingCallbackFinish;

	QTETimingCallback.BindUFunction(this, FName("UpdateTimingQTE"));
	QTETimingCallbackFinish.BindUFunction(this, FName("FinishQTE"));
	TimelineQTE.AddInterpFloat(CurveQTE, QTETimingCallback); 
	TimelineQTE.SetTimelineFinishedFunc(QTETimingCallbackFinish);
	
	UCurveFloat* _curve = NewObject<UCurveFloat>();
	_curve->FloatCurve.AddKey(0,0);
	_curve->FloatCurve.AddKey(DataAsset->BiteTiming, 1);
	
	FOnTimelineFloat CheckTimingCallback;
	FOnTimelineEventStatic CheckTimingCallbackFinish;
	TimelineWaitForBite.SetTimelineLengthMode(TL_TimelineLength);
	TimelineWaitForBite.SetTimelineLength(DataAsset->BiteTiming);
	CheckTimingCallback.BindUFunction(this, FName("UpdateBiteHoldHUD"));
	CheckTimingCallbackFinish.BindUFunction(this, FName("EndBite"));
	TimelineWaitForBite.AddInterpFloat(_curve, CheckTimingCallback); 
	TimelineWaitForBite.SetTimelineFinishedFunc( CheckTimingCallbackFinish);
	
	CanCheckForEnemy = true;

	RangeQTEBites.Add(OnGround, DataAsset->BiteGroundQTERange);
	RangeQTEBites.Add(InAir, DataAsset->BiteAirQTERange);
}

void UBiteSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TimelineWaitForBite.TickTimeline(DeltaTime);
	TimelineQTE.TickTimeline(DeltaTime);

	if(CurrentPlayerState->CurrentEnnemi.Get())
	{
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(CurrentController.Get(),CurrentPlayerState->CurrentEnnemi->GetActorLocation()+FVector::UpVector*80,OnScreenLoc,true);
		CurrentHUD->SetBiteInputPos(OnScreenLoc);

		if(!FinishedQTE && !IsBiting)
		{
			float opacity;
			switch (CurrentBiteType)
			{
				case OnLedge:
					CurrentHUD->SetQteRangeOpacity(0.8f);
					break;

				case InAir:
					opacity = FMath::GetMappedRangeValueClamped(FVector2d(DataAsset->BiteAirRange*2,DataAsset->BiteAirQTERange), FVector2d(DataAsset->BiteQteRangeMinOpacity,DataAsset->BiteQteRangeMaxOpacity), FVector::Distance(CurrentPlayerState->CurrentEnnemi->GetActorLocation(), ActorOwner->GetActorLocation()));
					CurrentHUD->SetQteRangeOpacity(opacity);
					break;
				
				case OnGround:
					opacity = FMath::GetMappedRangeValueClamped(FVector2d(DataAsset->BiteGroundRange*2,DataAsset->BiteGroundQTERange), FVector2d(DataAsset->BiteQteRangeMinOpacity,DataAsset->BiteQteRangeMaxOpacity), FVector::Distance(CurrentPlayerState->CurrentEnnemi->GetActorLocation(), ActorOwner->GetActorLocation()));
					CurrentHUD->SetQteRangeOpacity(opacity);
					break;
			}
		}
	}
	
	
	if(IsSucking && !IsFinishing && (LastType == OnGround || LastType == NightmareForm))
	{
		CurrentPlayerState->CurrentEnnemi->SetActorLocation(ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*15);
	}

	if(CanCheckForEnemy)
	{
		CheckEnemyForBite();
	}
}

void UBiteSystem::SetCurrentBiteType(BiteType type)
{
	if(CurrentPlayerState->IsInNightmareForm)
	{
		CurrentBiteType = NightmareForm;
		return;
	};
	CurrentBiteType = type;
}


void UBiteSystem::StartAttacking()
{
	if(!CurrentPlayerState->CurrentEnnemi.Get() || PivotIsAttacking || IsBiting || (Cast<ASoldier>(CurrentPlayerState->CurrentEnnemi.Get())->GetIsShielded() && !CurrentPlayerState->IsInNightmareForm)) return;
	CanCheckForEnemy = false;
	PivotIsAttacking = false;
	ExecuteQTE();
	IsBiting = true;
	IsTraveling = true;
	FVector offset;
	float Timing = 0.2f;
	
	switch (CurrentBiteType)
	{
		case OnLedge:
			offset = FVector::UpVector*100 + CurrentPlayerState->CurrentEnnemi->GetActorForwardVector()*100;
			Timing = DataAsset->OnLedgeBiteSpeed;
			IsNearForQTE = true;
			break;

		case InAir:
			CurrentHUD->SetQteRangeOpacity(0);
			offset = FVector::UpVector*140 +CurrentPlayerState->CurrentEnnemi->GetActorForwardVector()*100;
			break;

		case NightmareForm:
		case OnGround:
			CurrentHUD->SetQteRangeOpacity(0);
			offset = -CurrentPlayerState->CurrentEnnemi->GetActorForwardVector()*15;
			break;
	}

	IIBitable::Execute_BeingBite(CurrentPlayerState->CurrentEnnemi.Get(), 50);
	CurrentPlayerState->CurrentState = CantMove;
	LastType = CurrentBiteType;
	ActorOwner->StartTranslateCharacter(ActorOwner->GetActorLocation(), CurrentPlayerState->CurrentEnnemi->GetActorLocation()+offset, Timing);
	WasCrouched = CurrentPlayerState->IsCrouched;
	ActorOwner->SetCrouch(false);
	ActorOwner->SetActorRotation(CurrentPlayerState->CurrentEnnemi->GetActorRotation());
}

void UBiteSystem::StartBiting()
{
	if(!CurrentPlayerState->CurrentEnnemi.Get()) return;
	
	PlayerCameraManager->StartCameraShake(DataAsset->CameraShakeStartBite, 1);
	ActorOwner->SetBiteEffects(true);
	IsTraveling = false;
	CurrentPlayerState->CurrentState = CantMove;
	bool b = true;
	if(CurrentPlayerState->CurrentBloodGauge+1<=DataAsset->MaxBloodGauge)
	{
		CurrentHUD->SetActiveBlood(true);
	}
	switch (LastType)
	{
		case OnLedge:
			CurrentHUD->SetBiteText("<Silent>Silent</> Bite");
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(),DataAsset->DilatationTimeQTE);
			StartQTE();
			ActorOwner->GetCharacterMovement()->StopActiveMovement();
			ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("SetLedgeAttack")), &b);
			IIBitable::Execute_BeingArialBite(CurrentPlayerState->CurrentEnnemi.Get());
			break;
		
		case InAir:
			ActorOwner->GetCharacterMovement()->StopActiveMovement();
			ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("SetLedgeAttack")), &b);
			IIBitable::Execute_BeingArialBite(CurrentPlayerState->CurrentEnnemi.Get());
			CurrentHUD->SetPercentBiteHold(1);
			CurrentHUD->SetThicknessBiteHold(0.12);
			TimelineWaitForBite.PlayFromStart();
			CurrentHUD->SetColorBiteHold(FColor::Red);
			CurrentHUD->SetBiteText("<Type>(hold)</> <Feed>Feed</>");	
			break;

		case NightmareForm:
			EndBite();
		case OnGround:
			ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("SetGroundAttack")), &b);
			IIBitable::Execute_BeingAttacked(CurrentPlayerState->CurrentEnnemi.Get());
			CurrentHUD->SetPercentBiteHold(1);
			CurrentHUD->SetThicknessBiteHold(0.12);
			TimelineWaitForBite.PlayFromStart();

			CurrentHUD->SetColorBiteHold(FColor::Red);
			CurrentHUD->SetBiteText("<Type>(hold)</> <Feed>Feed</>");	
			break;
	}
}

void UBiteSystem::EndBite()
{
	if(!CurrentPlayerState->CurrentEnnemi.Get() || PivotIsAttacking || IsFinishing || !IsBiting || (LastType==OnLedge && IsBiting && !IsSucking && !StartedQTE && !FinishedQTE)) return;
	
	if(!IsSucking)
	{
		CurrentHUD->SetQteRangeOpacity(0);
		TimelineWaitForBite.Stop();
		CurrentHUD->SetActiveBiteInput(false);
		CurrentHUD->SetPercentBiteHold(1);
		CurrentHUD->SetBiteText("Bite");
		CurrentHUD->SetColorBiteHold(FColor::White);
		PivotIsAttacking = true;
		return;
	}
	
	Percent = TimelineWaitForBite.GetPlaybackPosition()/DataAsset->BiteTiming *100;
	CurrentHUD->SetActiveBlood(false);
	if (Percent < 100)
	{
		ShortBite();
	}
	else if(Percent==100)
	{
		LongBite();
	}
}

void UBiteSystem::SetAnimLedgeBite(bool b)
{
	ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("SetLedgeAttack")), &b);
}

void UBiteSystem::UpdateBiteHoldHUD(float Value)
{
	CurrentHUD->SetPercentBiteHold(1-Value);
	if(CurrentPlayerState->CurrentBloodGauge+1<=DataAsset->MaxBloodGauge)
	{
		CurrentController->BlindSkill->SetCurrentUIBlood(CurrentPlayerState->CurrentBloodGauge);
		CurrentController->BlindSkill->UpdateUICurrentBlood(Value);
	}
}

void UBiteSystem::OnGroundBite()
{
	bool b = true;
	bool f = false;

	CurrentPlayerState->CurrentState = CanOnlyMove;

	ActorOwner->GetCharacterMovement()->bOrientRotationToMovement = false;
	ActorOwner->SetModifier(0.2);
	
	ActorOwner->SetBlendMask(true);
	ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("SetIsBiting")),&b);
	ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("SetGroundAttack")),&f);
	
	IsSucking = true;
	
	if(PivotIsAttacking)
	{
		ActorOwner->GetWorldTimerManager().SetTimer(PivotAttackingTimerHandle,this, &UBiteSystem::ShortBite,0.1f,false);
	}	
}

void UBiteSystem::OnLedgeBite()
{
	bool b = true;
	CurrentPlayerState->CurrentState = CantMove;
	if(LastType == OnLedge)
	{
		CurrentHUD->SetPercentBiteHold(1);
		CurrentHUD->SetThicknessBiteHold(0.1);
		TimelineWaitForBite.PlayFromStart();

		CurrentHUD->SetColorBiteHold(FColor::Red);
		CurrentHUD->SetBiteText("<Type>(hold)</> <Feed>Feed</>");	
	}
	ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("SetIsBiting")),&b);
	IsSucking = true;
	
	if(PivotIsAttacking)
	{
		ActorOwner->GetWorldTimerManager().SetTimer(PivotAttackingTimerHandle,this, &UBiteSystem::ShortBite,0.1f,false);
	}
}

void UBiteSystem::ToggleShowBiteRange()
{
	IsRangeVisible = !IsRangeVisible;
}

void UBiteSystem::ShortBite()
{
	TimelineWaitForBite.Stop();
	bool b = false;
	ActorOwner->SetBiteEffects(false);
	ExecuteBite();
	ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("SetIsBiting")), &b);
}

void UBiteSystem::LongBite()
{
	if(CurrentPlayerState->CurrentBloodGauge+1<=DataAsset->MaxBloodGauge)
	{
		CurrentPlayerState->CurrentBloodGauge++;
	}
	ActorOwner->SetBiteEffects(false);
	PlayerCameraManager->StartCameraShake(DataAsset->CameraShakeEndLongBite, 1);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1f);
	ActorOwner->CustomTimeDilation = 10.f;
	CurrentPlayerState->CurrentEnnemi->CustomTimeDilation = 10.f;
	CurrentHUD->SetActiveBiteInput(false);
	TimelineWaitForBite.Stop();
	IsFinishing = true;
	CurrentPlayerState->CurrentState = CantMove;
	switch (LastType)
	{
		case OnLedge:
			ActorOwner->FinisherBite();
			ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("ToggleFinisher")), NULL);
			CurrentPlayerState->CurrentEnnemi->SetActorLocation(ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*60);
			Cast<ASoldier>(CurrentPlayerState->CurrentEnnemi)->IsFinishing();
			break;
			
		case InAir:
			ActorOwner->FinisherBite();
			ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("ToggleFinisher")), NULL);
			CurrentPlayerState->CurrentEnnemi->SetActorLocation(ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*60);
			Cast<ASoldier>(CurrentPlayerState->CurrentEnnemi)->IsFinishing();
			break;

		case NightmareForm:
		case OnGround:
			ActorOwner->FinisherBite();
			ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("ToggleFinisher")), NULL);
			CurrentPlayerState->CurrentEnnemi->SetActorLocation(ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*60);
			Cast<ASoldier>(CurrentPlayerState->CurrentEnnemi)->IsFinishing();
			break;
	}
}

void UBiteSystem::CheckInputQTE()
{
	if(LastType==OnLedge && StartedQTE && IsBiting)
	{
		ExecuteQTE();
		return;
	}

	StartAttacking();
}


void UBiteSystem::ExecuteBite()
{
	CurrentController->BlindSkill->IncreaseBlood();
	CurrentController->ClientStopForceFeedback(VibroDataAsset->BiteEffect, FName(""));
	CurrentPlayerState->CurrentState = CantMove;
	IIBitable::Execute_Die(CurrentPlayerState->CurrentEnnemi.Get());

	IsQTETiming = false;
	PivotIsAttacking = false;
	IsSucking = false;
	IsFinishing = false;
	CurrentPlayerState->CurrentEnnemi= nullptr;
	FinishedQTE = false;
	
	ActorOwner->SetModifier(1);
	ActorOwner->GetCharacterMovement()->bOrientRotationToMovement = true;

	CanCheckForEnemy = true;
	IsBiting = false;
	
	CurrentHUD->SetPercentBiteHold(1);
	CurrentHUD->SetBiteText("Bite");
	CurrentHUD->SetColorBiteHold(FColor::White);

	if(CurrentPlayerState->IsInNightmareForm)
	{
		EnemiEndAlive--;
		ActorOwner->CheckEndGame(EnemiEndAlive);
	}
}

void UBiteSystem::FinisherLongBite()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
	ActorOwner->CustomTimeDilation = 1;
	CurrentPlayerState->CurrentEnnemi->CustomTimeDilation = 1;
	bool b = false;
	
	CurrentController->SetControlRotation(ActorOwner->GetActorRotation());
	ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("SetIsBiting")), &b);
	ActorOwner->CurrentAnims->ProcessEvent(ActorOwner->CurrentAnims->FindFunction(FName("ToggleFinisher")), NULL);
	ActorOwner->FinisherBite();

	if(IsQTETiming)
	{
		IIBitable::Execute_BeingDissolved(CurrentPlayerState->CurrentEnnemi.Get());
	}

	ExecuteBite();
	ActorOwner->SetModifier(1);
	ActorOwner->GetCharacterMovement()->bOrientRotationToMovement = true;
	UnfreezePlayer();
}

void UBiteSystem::CheckEnemyForBite()
{
	FHitResult RayResult;
	FVector test2;
	FBox test5;
	switch (CurrentBiteType)
	{
		case OnGround:
			Shape = FCollisionShape::MakeBox(FVector(DataAsset->BiteGroundRange,DataAsset->BiteGroundWidth,10));
			ActorOwner->GetWorld()->SweepSingleByChannel(RayResult, ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*DataAsset->BiteGroundRange, ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*DataAsset->BiteGroundRange, ActorOwner->GetActorRotation().Quaternion(), ECC_GameTraceChannel1, Shape, ParamRaycast);
			
			if(IsRangeVisible)
			{
				test5 = FBox(FVector(-DataAsset->BiteGroundRange,-DataAsset->BiteGroundWidth,-10), FVector(DataAsset->BiteGroundRange,DataAsset->BiteGroundWidth,10));
				DrawDebugSolidBox(GetWorld(), test5, FColor::Blue, FTransform(ActorOwner->GetActorRotation(), ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*DataAsset->BiteGroundRange), false, 0.1f);
			}
			break;

		case OnLedge:
			test2 = FVector(PlayerCameraManager->GetCameraRotation().Vector().X,PlayerCameraManager->GetCameraRotation().Vector().Y,0);
			test2.Normalize();
		
			Shape = FCollisionShape::MakeCapsule(200,500);
			ActorOwner->GetWorld()->SweepSingleByChannel(RayResult,
			ActorOwner->GetActorLocation()+test2*200-FVector::UpVector*800,
			ActorOwner->GetActorLocation()+test2*200-FVector::UpVector*800,
			ActorOwner->GetActorRotation().Quaternion(), ECC_GameTraceChannel1, Shape, ParamRaycast);

			if(IsRangeVisible)
			{
				DrawDebugCapsule(GetWorld(), ActorOwner->GetActorLocation()+test2*200-FVector::UpVector*800, 500,200,ActorOwner->GetActorRotation().Quaternion(), FColor::Blue, false, .1);
			}
		
			break;

		case InAir:
			Shape = FCollisionShape::MakeBox(FVector(DataAsset->BiteAirRange,DataAsset->BiteAirWidth,DataAsset->BiteAirHeight));
			ActorOwner->GetWorld()->SweepSingleByChannel(RayResult, ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*DataAsset->BiteAirRange-ActorOwner->GetActorUpVector()*DataAsset->BiteAirHeight, ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*DataAsset->BiteAirRange-ActorOwner->GetActorUpVector()*DataAsset->BiteAirHeight, ActorOwner->GetActorRotation().Quaternion(), ECC_GameTraceChannel1, Shape, ParamRaycast);
			
			if(IsRangeVisible)
			{
				test5 = FBox(FVector(-DataAsset->BiteAirRange,-DataAsset->BiteAirWidth,-DataAsset->BiteAirHeight), FVector(DataAsset->BiteAirRange,DataAsset->BiteAirWidth,DataAsset->BiteAirHeight));
				DrawDebugSolidBox(GetWorld(), test5, FColor::Purple, FTransform(ActorOwner->GetActorRotation(), ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*DataAsset->BiteAirRange-ActorOwner->GetActorUpVector()*DataAsset->BiteAirHeight), false, 0.1f);
			}
			break;
		case NightmareForm:
			Shape = FCollisionShape::MakeBox(FVector(DataAsset->BiteGroundRange*10,DataAsset->BiteGroundWidth,10));
			ActorOwner->GetWorld()->SweepSingleByChannel(RayResult, ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*DataAsset->BiteGroundRange*10, ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*DataAsset->BiteGroundRange*10, ActorOwner->GetActorRotation().Quaternion(), ECC_GameTraceChannel1, Shape, ParamRaycast);
			
			if(IsRangeVisible)
			{
				test5 = FBox(FVector(-DataAsset->BiteGroundRange*10,-DataAsset->BiteGroundWidth,-10), FVector(DataAsset->BiteGroundRange*10,DataAsset->BiteGroundWidth,10));
				DrawDebugSolidBox(GetWorld(), test5, FColor::Blue, FTransform(ActorOwner->GetActorRotation(), ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*DataAsset->BiteGroundRange*10), false, 0.1f);
			}
			break;
	}
	
	if(RayResult.bBlockingHit)
	{
		if(RayResult.HitObjectHandle.GetRepresentedClass()->ImplementsInterface(UIBitable::StaticClass()))
		{
			ActorOwner->GetWorld()->LineTraceSingleByChannel(RaycastResult, ActorOwner->GetActorLocation()+(FVector::UpVector*ActorOwner->CapsuleHeight), RayResult.HitObjectHandle.FetchActor()->GetActorLocation(), ECC_WorldDynamic, ParamRaycast);
			
			if(RaycastResult.HitObjectHandle.FetchActor() == RayResult.HitObjectHandle.FetchActor())
			{
				ASoldier* Soldier = Cast<ASoldier>(RayResult.GetActor());
				
				ActorOwner->CurrentPlayerState->CurrentEnnemi = RayResult.GetActor();
				
				CurrentHUD->SetActiveBiteInput(true);
				
				if(!FirstCheckNearQTE && CurrentBiteType!=OnLedge)
				{
					CurrentHUD->SetActiveQTERange(true);
				}
				
				if(!Soldier->GetIsShielded())
				{
					
					CurrentHUD->SetActiveBiteCross(false);
					if(CurrentBiteType!=OnLedge && CurrentBiteType!=NightmareForm)
					{
						if(FVector::Distance(CurrentPlayerState->CurrentEnnemi->GetActorLocation(), ActorOwner->GetActorLocation())<=RangeQTEBites[CurrentBiteType])
						{
							if(!FirstCheckNearQTE)
							{
								if(!IsNearForQTE)
								{
									if(CurrentBiteType== InAir)
									{
										UGameplayStatics::SetGlobalTimeDilation(GetWorld(), DataAsset->DilatationTimeQTE);
									}
									FirstCheckNearQTE = true;
									IsNearForQTE = true;
									CurrentHUD->SetBiteText("<Silent>Silent</> Bite");
									StartQTE();
								}
							}
						}else
						{
							FirstCheckNearQTE = false;
							IsNearForQTE = false;
						}
					}
				}else
				{
					if(!CurrentPlayerState->IsInNightmareForm)
					{
						CurrentHUD->SetActiveBiteCross(true);
						CurrentHUD->SetBiteText("<Type>Bite</>");
					}
				}
					
				return;
			}
		}
	}

	if(StartedQTE)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(),1);
	}

	StopQTE();
	ActorOwner->CurrentPlayerState->CurrentEnnemi = nullptr;
	IsNearForQTE = false;
	CurrentHUD->SetActiveBiteCross(false);
	CurrentHUD->SetActiveBiteInput(false);
	
}

void UBiteSystem::StartQTE()
{
	TimelineQTE.Stop();
	if(!IsBiting)
	{
		if(CurrentBiteType == OnGround)
		{
			TimelineQTE.SetPlayRate(1);
		}else if(CurrentBiteType== InAir)
		{
			TimelineQTE.SetPlayRate(1/DataAsset->DilatationTimeQTE);
		}
	}else
	{
		if(LastType == OnLedge)
		{
			TimelineQTE.SetPlayRate(1/DataAsset->DilatationTimeQTE);
		}
	}
	
	FinishedQTE = false;
	StartedQTE = true;
	IsQTETiming = false;
	CurrentHUD->SetActiveQTERange(true);
	CurrentHUD->SetBiteBarScale(1.5f);
	TimelineQTE.PlayFromStart();
}

void UBiteSystem::StopQTE()
{
	CurrentHUD->SetColorBiteHold(FColor::White);
	StartedQTE = false;
	TimelineQTE.Stop();
	CurrentHUD->SetQteRangeOpacity(0);
	CurrentHUD->SetBiteQteTiming(false);
	CurrentHUD->SetBiteBarScale(1);
	CurrentHUD->SetBiteText("Bite");
}

void UBiteSystem::ExecuteQTE()
{
	if(!IsNearForQTE) return;
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(),1);
	FinishedQTE = true;
	StopQTE();

	CurrentHUD->QteSuccedAnimation(IsQTETiming);
}

void UBiteSystem::FinishQTE()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(),1);
	CurrentHUD->SetQteRangeOpacity(0);
	StartedQTE = false;
	CurrentHUD->SetColorBiteHold(FColor::White);
	CurrentHUD->SetBiteText("Bite");
	IsQTETiming = false;
	CurrentHUD->SetBiteBarScale(1);
	CurrentHUD->SetBiteQteTiming(false);
	FinishedQTE = true;
	CurrentHUD->QteSuccedAnimation(IsQTETiming);
	
	if(IsBiting && LastType == OnLedge)
	{
		PivotIsAttacking = true;
		CurrentPlayerState->CurrentState = CantMove;
	}else
	{
		CurrentPlayerState->CurrentState = NormalState;
	}
}

void UBiteSystem::UpdateTimingQTE(float Value)
{
	CurrentHUD->SetBiteBarScale(FMath::Lerp(3,0.7, Value));

	if(!IsQTETiming && Value>=0.6 && Value<=0.9)
	{
		IsQTETiming = true;
		CurrentHUD->SetColorBiteHold(FColor::Yellow);
		CurrentHUD->SetBiteQteTiming(true);
	}else if(Value>0.9)
	{
		IsQTETiming = false;
		CurrentHUD->SetColorBiteHold(FColor::White);
		CurrentHUD->SetBiteQteTiming(false);
	}
}

void UBiteSystem::PlayBiteSound()
{
	IIBitable::Execute_BiteSound(CurrentPlayerState->CurrentEnnemi.Get(), IsQTETiming, !PivotIsAttacking);
}

void UBiteSystem::UnfreezePlayer()
{
	CurrentHUD->SetThicknessBiteHold(0.02);
	CurrentPlayerState->CurrentState = NormalState;
	ActorOwner->SetCrouch(WasCrouched);
}
