// Fill out your copyright notice in the Description page of Project Settings.


#include "HookSystem.h"

#include "AkGameplayStatics.h"
#include "BloodRenegade/Controller/BloodCharacter.h"
#include "BloodRenegade/Controller/BloodCharacterDataAsset.h"
#include "BloodRenegade/Controller/BloodPlayerState.h"
#include "BloodRenegade/Controller/BloodController.h"
#include "BloodRenegade/Controller/BloodHUD.h"
#include "BloodRenegade/WorldActors/CustomSpringArm.h"
#include "BloodRenegade/ActorComponents/VampiricVisionComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"

UHookSystem::UHookSystem()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHookSystem::BeginPlay()
{
	Super::BeginPlay();
	ActorOwner = GetOwner<ABloodCharacter>();
	CurrentPlayerState = ActorOwner->GetPlayerState<ABloodPlayerState>();
	CurrentController = ActorOwner->GetController<ABloodController>();
	CurrentHud = ActorOwner->GetController<ABloodController>()->GetHUD<ABloodHUD>();

	ParamRaycast.AddIgnoredActor(ActorOwner.Get());
	CanHook = true;

	ResetCamCurv = NewObject<UCurveFloat>();
	ResetCamCurv->FloatCurve.AddKey(0,0);
	ResetCamCurv->FloatCurve.AddKey(0.6f,1);

	FOnTimelineFloat ResetCamCallBack;
	ResetCamCallBack.BindUFunction(this, FName("LerpResetCamera"));
	ResetCamTimeline.AddInterpFloat(ResetCamCurv, ResetCamCallBack);
}

void UHookSystem::Hook()
{
	if(!IsInSolarLight && FVector::Distance(CurrentPlayerState->HookTarget->GetActorLocation(), ActorOwner->GetActorLocation()) <= DataAsset->HookRange)
	{
		ActorOwner->NewVampiricVisionComponent->StopVision();
		ResetCamDone = false;
		IsHooking = true;
		ActorOwner->CameraStick->CameraLagSpeed = 3;
		ActorOwner->CameraStick->CameraLagMaxDistance = 0;
		ActorOwner->SetMaxFOV(125);
		ActorOwner->ModifFovTimeline.PlayFromStart();
		ActorOwner->GetWorldTimerManager().SetTimer(EnableTimerHandle, this, &UHookSystem::EnableHook, DataAsset->HookCooldown + DataAsset->HookSpeed, false);
		ActorOwner->StartTranslateCharacter(ActorOwner->GetActorLocation(), CurrentPlayerState->HookTarget->GetActorLocation()+FVector::UpVector*ActorOwner->CapsuleHeight, DataAsset->HookSpeed);
		CanHook = false;
	}else
	{
		CurrentHud->CantHook();
	}
}

void UHookSystem::ResetCamHook()
{
	ResetCamDone = true;
	LastCamRotation = CurrentController->PlayerCameraManager->GetCameraRotation();
	NextCamRotation = ActorOwner->GetActorRotation()+FRotator(-30,0,0);
	ActorOwner->ModifFovTimeline.Stop();
	//ActorOwner->CameraStick->SetFullLerping(false);
	ResetCamTimeline.PlayFromStart();
}

void UHookSystem::FinishHook()
{
	CurrentPlayerState->CurrentState = CantMove;
	IsHooking = false;
	TWeakObjectPtr<UHookSystem> WeakThis = this;
	ActorOwner->GetWorldTimerManager().SetTimer(FinishHookTimerHandle, FTimerDelegate::CreateWeakLambda(this, 
		   [WeakThis]
		   {
			   if(WeakThis.IsValid())
			   {
					WeakThis->CurrentPlayerState->CurrentState = NormalState;
			   }
		   }), 0.7f, false);
}

void UHookSystem::EnableHook()
{
	CanHook = true;
}

void UHookSystem::LerpResetCamera(float Value)
{
	ActorOwner->FollowCamera->FieldOfView = FMath::Lerp(125,90,Value);
	CurrentController->SetControlRotation(FMath::Lerp(LastCamRotation,NextCamRotation, Value));
}

TWeakObjectPtr<AActor> UHookSystem::CheckPointClosest(TArray<FHitResult> Results)
{
	if(!CanHook) return nullptr;
	FHitResult CloserHook;
	FVector CamLoc = CurrentController->PlayerCameraManager->GetCameraLocation();
	int32 vpX, vpY, index = 0;
	vpX = GSystemResolution.ResX;
	vpY = GSystemResolution.ResY;
	FVector2D ScreenView = FVector2D(vpX/2, (vpY/2));
	FVector2D ScreenLoc, HookScreenLoc;
	
	for(int i = 0; i < Results.Num(); i++)
	{
		UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(CurrentController.Get(),Results[i].GetActor()->GetActorLocation(),ScreenLoc, true);

		if(ScreenLoc.X>10 && ScreenLoc.X<vpX && ScreenLoc.Y>10 && ScreenLoc.Y<vpY)
		{
			if(FVector::Distance(ActorOwner->GetActorLocation(),Results[i].GetActor()->GetActorLocation())>DataAsset->HookMinimumDistance)
			{
				if(CloserHook.GetActor())
				{
					UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(CurrentController.Get(),CloserHook.GetActor()->GetActorLocation(),HookScreenLoc,true);

					if(FVector2D::Distance(ScreenLoc, ScreenView) < FVector2D::Distance(HookScreenLoc, ScreenView))
					{
						CloserHook = Results[i];
						index = i;
					}
				}else
				{
					CloserHook = Results[i];
					index = i;
				}
			}
		}
	}

	if(CloserHook.GetActor())
	{
		FHitResult CheckBehindSomething;
		GetWorld()->LineTraceSingleByChannel(CheckBehindSomething, ActorOwner->GetActorLocation()+FVector::UpVector*ActorOwner->CapsuleHeight, CloserHook.GetActor()->GetActorLocation(), ECC_WorldStatic, ParamRaycast);
		
		if(CheckBehindSomething.GetActor() == CloserHook.GetActor())
		{
			LastDireRecord = (CloserHook.GetActor()->GetActorLocation() - CamLoc).Rotation();
			LastDireRecord.Pitch=0;
			return CloserHook.GetActor();
		}
		
		Results.RemoveAt(index);
		
		if(Results.Num()==0)
		{
			return nullptr;
		}

		return CheckPointClosest(Results);
	}

	return nullptr;
}

void UHookSystem::CheckHookPoint()
{
	TArray<FHitResult> Results;
	FCollisionShape Shape = FCollisionShape::MakeSphere(DataAsset->HookMaxVisible);
	GetWorld()->SweepMultiByChannel(Results, ActorOwner->GetActorLocation(), ActorOwner->GetActorLocation(), ActorOwner->GetActorRotation().Quaternion() ,ECC_GameTraceChannel2, Shape, ParamRaycast);
	//DrawDebugSphere(GetWorld(), ActorOwner->GetActorLocation(), 1500, 100, FColor::Black, false, 0.1f);  
	
	if(Results.Num() == 0)
	{
		CurrentPlayerState->HookTarget = nullptr;
		CurrentController->CurrentHUD->SetActiveHookWidget(false);
	}else
	{
		CurrentPlayerState->HookTarget = CheckPointClosest(Results);
		
		if(CurrentPlayerState->HookTarget.Get())
		{
			float f2 = FVector::Distance(CurrentPlayerState->HookTarget->GetActorLocation(), ActorOwner->GetActorLocation()) - DataAsset->HookRange;

			FVector2D HookScreenLoc;
			UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(CurrentController.Get(),CurrentPlayerState->HookTarget->GetActorLocation(),HookScreenLoc,true);
			CurrentController->CurrentHUD->SetActiveHookWidget(true);
			CurrentController->CurrentHUD->UpdateHookPosition(HookScreenLoc, 1-(f2 / 300));
		}else
		{
			CurrentController->CurrentHUD->SetActiveHookWidget(false);
		}
	}
}

void UHookSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ResetCamTimeline.TickTimeline(DeltaTime);
	
	CheckHookPoint();
}

