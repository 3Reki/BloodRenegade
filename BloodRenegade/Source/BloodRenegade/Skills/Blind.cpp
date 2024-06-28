// Fill out your copyright notice in the Description page of Project Settings.


#include "Blind.h"
#include "BloodRenegade/DataAssets/Sound/CharacterSoundDataAsset.h"
#include "AkGameplayStatics.h"
#include "AkGameplayTypes.h"
#include "BloodRenegade/Controller/BloodHUD.h"
#include "BloodRenegade/Controller/BloodCharacter.h"
#include "BloodRenegade/Controller/BloodController.h"
#include "BloodRenegade/Controller/BloodCharacterDataAsset.h"
#include "BloodRenegade/Controller/BloodPlayerState.h"
#include "BloodRenegade/Interfaces/IBlindable.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBlind::BeginPlay()
{
	Super::BeginPlay();
	
	HUD = GetOwner<ABloodController>()->CurrentHUD;
	Pawn = Cast<ABloodCharacter>(GetOwner<ABloodController>()->GetCharacter());
	CollisionParams.AddIgnoredActor(Pawn.Get());
	CurrentPlayerState = Pawn->GetPlayerState<ABloodPlayerState>();
	ParamRaycast.AddIgnoredActor(Pawn.Get());
	
	UCurveFloat* DecreaseBloodCurve = NewObject<UCurveFloat>();
	DecreaseBloodCurve->FloatCurve.AddKey(0,1);
	DecreaseBloodCurve->FloatCurve.AddKey(0.2f,0);

	FOnTimelineFloat DecreaseBloodCallBack;
	DecreaseBloodCallBack.BindUFunction(this, FName("UpdateUICurrentBlood"));
	TimelineDecreaseBlood.AddInterpFloat(DecreaseBloodCurve, DecreaseBloodCallBack);
	
	UCurveFloat* FirstBloodCurve = NewObject<UCurveFloat>();
	FirstBloodCurve->FloatCurve.AddKey(0,0);
	FirstBloodCurve->FloatCurve.AddKey(1.0f,1);

	FOnTimelineFloat FirstBloodCallBack;
	FOnTimelineEventStatic FirstBloodCallBackStatic;
	FirstBloodCallBack.BindUFunction(this, FName("UpdateUICurrentBlood"));
	FirstBloodCallBackStatic.BindUFunction(this, FName("UnableFixBlood"));
	TimelineIncreaseBlood.AddInterpFloat(FirstBloodCurve, FirstBloodCallBack);
	TimelineIncreaseBlood.SetTimelineFinishedFunc(FirstBloodCallBackStatic);
}


void UBlind::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TimelineIncreaseBlood.TickTimeline(DeltaTime);
	TimelineDecreaseBlood.TickTimeline(DeltaTime);

	for(int i =0; i<CurrentPlayerState->CurrentTargetEnemy.Num();i++)
	{
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetOwner<ABloodController>(),CurrentPlayerState->CurrentTargetEnemy[i]->GetActorLocation()+ FVector::UpVector*IIBlindable::Execute_GetBlindOffset(CurrentPlayerState->CurrentTargetEnemy[i].Get()),ScreenPos,true);
		HUD->SetTargetPos(i, ScreenPos);
	}

	if(IsUsing && CurrentPlayerState->IsCrouched)
	{
		GetWorld()->LineTraceSingleByChannel(Result,
				Pawn->GetActorLocation()+Pawn->GetActorUpVector()*Pawn->CapsuleHeight,
				Pawn->GetActorLocation()+Pawn->GetActorUpVector()*Pawn->CapsuleHeight+Pawn->GetActorForwardVector()*70,
				ECC_WorldStatic, ParamRaycast);
		if(Result.bBlockingHit)
		{
			GetWorld()->LineTraceSingleByChannel(Result,
				Pawn->GetActorLocation()+Pawn->GetActorUpVector()*Pawn->CapsuleHeight+30,
				Pawn->GetActorLocation()+Pawn->GetActorUpVector()*Pawn->CapsuleHeight+30+Pawn->GetActorForwardVector()*70,
				ECC_WorldStatic, ParamRaycast);

			if(!Result.bBlockingHit)
			{
				WasCrouch = true;
				Pawn->SetCrouch(false);
			}
		}
	}
}

void UBlind::Start()
{
	IsUsing = true;
	HUD->SetActiveAim(true);
	HUD->SetActiveBlood(true);
	PreviewBloodGauge = CurrentPlayerState->CurrentBloodGauge;
}


void UBlind::Invoke()
{
	if (Pawn->BloodGaugeEnable && CurrentPlayerState->CurrentTargetEnemy.Num()>0)
	{
		
		for (auto Element : CurrentPlayerState->CurrentTargetEnemy)
		{
			IIBlindable::Execute_BeingBlind(Element.Get(), Pawn->DataAsset->BlindEnemyTimer);
			Pawn->EffectTargetBlindLock(Element->GetActorLocation());
		}

		const FOnAkPostEventCallback NullCallback;
		UAkGameplayStatics::PostEvent(Pawn->SoundDataAsset->BlindInvoke, Pawn.Get(), 0, NullCallback);

		CurrentPlayerState->CurrentBloodGauge = PreviewBloodGauge;
		HUD->UpdateAllBlood(CurrentPlayerState->CurrentBloodGauge);
		
		if(PreviewBloodGauge==0)
		{
			HUD->SetActiveBlood(true);
			GetWorld()->GetTimerManager().ClearTimer(FirstBloodTimerHandle);
			TWeakObjectPtr<UBlind> WeakThis = this;
			GetWorld()->GetTimerManager().SetTimer(FirstBloodTimerHandle, FTimerDelegate::CreateWeakLambda(this, 
			[WeakThis]
			{
				if(WeakThis.IsValid())
				{
					WeakThis->CurrentPlayerState->CurrentBloodGauge++;
					WeakThis->IncreaseUIBlood(0);
				}
			}), DataAsset->TimerAddFirstBlood, false);
		}
	}else
	{
		HUD->CantSkill();
		HUD->SetPercentCrossHair(0);
	}

	ResetSkill();
}

void UBlind::ResetSkill()
{
	for(int i =0;i<4;i++)
	{
		HUD->SetActiveTarget(i, false);
	}
	
	if(WasCrouch)
	{
		Pawn->SetCrouch(WasCrouch);
	}

	HUD->SetActiveBlood(false);
	IsUsing = false;
	Pawn->GetWorldTimerManager().ClearTimer(AddTargetTimerHandler);
	HUD->TargetInRangeAIM(false);
	HUD->SetActiveAim(false);
	CurrentPlayerState->CurrentTargetEnemy.Empty();
	LastTarget = nullptr;
}

void UBlind::Cancel()
{
	ResetSkill();	
	HUD->UpdateAllBlood(CurrentPlayerState->CurrentBloodGauge);
}

void UBlind::PreviewSkillTarget()
{
	if(PreviewBloodGauge>0)
	{
		const APlayerCameraManager* PlayerCameraManager = GetOwner<ABloodController>()->PlayerCameraManager;
		GetWorld()->LineTraceSingleByChannel(RaycastResult, PlayerCameraManager->GetCameraLocation(),
		PlayerCameraManager->GetCameraLocation() + PlayerCameraManager->GetCameraRotation().Vector() * DataAsset->SkillPerimeterMaxRange,
		ECC_WorldStatic, CollisionParams);
		//DrawDebugLine(GetWorld(),GetOwner<ABloodController>()->PlayerCameraManager->GetCameraLocation(), GetOwner<ABloodController>()->PlayerCameraManager->GetCameraLocation()+GetOwner<ABloodController>()->PlayerCameraManager->GetCameraRotation().Vector()*_pawn->_dataAsset.skillPerimeterRange, FColor::Green, false, 0.5f,0,2);

		if(RaycastResult.HitObjectHandle.GetRepresentedClass()->ImplementsInterface(UIBlindable::StaticClass()))
		{
			AActor* Pivot = RaycastResult.HitObjectHandle.FetchActor();
			float dist = FVector::Distance(Pivot->GetActorLocation(), PlayerCameraManager->GetCameraLocation());
		
			if(CurrentPlayerState->CurrentTargetEnemy.Contains(Pivot) || Pivot == LastTarget) return;
			
			if(dist - DataAsset->SkillPerimeterRange<=0)
			{
				Pawn->GetWorldTimerManager().ClearTimer(AddTargetTimerHandler);
				HUD->TargetInRangeAIM(true);
				Pawn->GetWorldTimerManager().SetTimer(AddTargetTimerHandler, this, &UBlind::AddActorToTarget, DataAsset->TimerForAddTargetToBlind*UGameplayStatics::GetGlobalTimeDilation(Pawn->GetWorld()), false);
				LastTarget = Pivot;
			}

			float RemapPercent = FMath::GetMappedRangeValueClamped(FVector2d(DataAsset->SkillPerimeterMaxRange,DataAsset->SkillPerimeterRange), FVector2d(0,1), dist);
			HUD->SetPercentCrossHair(RemapPercent);
		}else
		{
			Pawn->GetWorldTimerManager().ClearTimer(AddTargetTimerHandler);
			LastTarget = nullptr;
			GetOwner<ABloodController>()->CurrentHUD->TargetInRangeAIM(false);
			HUD->SetPercentCrossHair(0);
		}
	}
}

void UBlind::SetCurrentUIBlood(float f)
{
	CurrentUiBlood = f;
}

void UBlind::UpdateUICurrentBlood(float Value)
{
	HUD->UpdateCurrentBlood(CurrentUiBlood,Value);
}

void UBlind::StartDecreaseBlood()
{
	TWeakObjectPtr<UBlind> WeakThis = this;
	GetWorld()->GetTimerManager().SetTimer(DecreasingHpTimerHandle, FTimerDelegate::CreateWeakLambda(this, 
		   [WeakThis]
		   {
			   if(WeakThis.IsValid())
			   {
			   	   if( WeakThis->CurrentPlayerState->CurrentBloodGauge-1>=0)
			   	   {
			   	   		WeakThis->CurrentPlayerState->CurrentBloodGauge-=1;
			   	   		WeakThis->DecreaseUIBlood(WeakThis->CurrentPlayerState->CurrentBloodGauge);
			   	   }else
			   	   {
			   	   		WeakThis->StopDecreaseBlood();
			   	   }
			   }
		   }), DataAsset->TimerDecreaseBlood, true);
}

void UBlind::StopDecreaseBlood()
{
	GetWorld()->GetTimerManager().ClearTimer(DecreasingHpTimerHandle);
	if(CurrentPlayerState->CurrentBloodGauge==0)
	{
		HUD->SetActiveBlood(true);
		CurrentPlayerState->CurrentBloodGauge++;
		IncreaseUIBlood(0);
	}
}

void UBlind::IncreaseBlood()
{
	if(CurrentPlayerState->CurrentBloodGauge+1>DataAsset->MaxBloodGauge)
	{
		CurrentPlayerState->CurrentBloodGauge = DataAsset->MaxBloodGauge;
	}else
	{
		CurrentPlayerState->CurrentBloodGauge++;
		CurrentUiBlood = CurrentPlayerState->CurrentBloodGauge-1;
		IncreaseUIBlood(CurrentUiBlood);
	}
}


void UBlind::IncreaseUIBlood(float f)
{
	if(TimelineIncreaseBlood.IsPlaying())
	{
		HUD->UpdateCurrentBlood(CurrentUiBlood,1);
	}
	
	CurrentUiBlood = f;
	TimelineIncreaseBlood.PlayFromStart();
}

void UBlind::DecreaseUIBlood(float f)
{
	if(TimelineDecreaseBlood.IsPlaying())
	{
		HUD->UpdateCurrentBlood(CurrentUiBlood,1);
	}
	
	CurrentUiBlood = f;
	TimelineDecreaseBlood.PlayFromStart();
}

void UBlind::UnableFixBlood()
{
	HUD->SetActiveBlood(false);
}


void UBlind::AddActorToTarget()
{
	PreviewBloodGauge--;
	CurrentPlayerState->CurrentTargetEnemy.Add(LastTarget);
	HUD->SetActiveTarget(CurrentPlayerState->CurrentTargetEnemy.Num()-1, true);
	LastTarget = nullptr;
	DecreaseUIBlood(PreviewBloodGauge);
}


