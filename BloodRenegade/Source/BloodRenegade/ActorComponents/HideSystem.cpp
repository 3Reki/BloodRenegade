// Fill out your copyright notice in the Description page of Project Settings.


#include "HideSystem.h"
#include "BloodRenegade/Controller/BloodCharacter.h"
#include "BloodRenegade/Controller/BloodPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BloodRenegade/Controller/BloodController.h"
#include "BloodRenegade/WorldActors/CustomSpringArm.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
UHideSystem::UHideSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHideSystem::BeginPlay()
{
	Super::BeginPlay();

	ActorOwner = GetOwner<ABloodCharacter>();
	CurrentPlayerState = GetOwner<ABloodCharacter>()->GetPlayerState<ABloodPlayerState>();
	CurrentController = GetOwner<ABloodCharacter>()->GetController<ABloodController>();
	PlayerCameraManager = ActorOwner->GetController<ABloodController>()->PlayerCameraManager;
	ParamRaycast.AddIgnoredActor(ActorOwner.Get());
	
	FOnTimelineEventStatic CheckWalkCallbackFinish;
	TimelineCheckWall.SetTimelineLengthMode(TL_TimelineLength);
	TimelineCheckWall.SetTimelineLength(0.05f);
	CheckWalkCallbackFinish.BindUFunction(this, FName("CheckWall"));
	TimelineCheckWall.SetTimelineFinishedFunc( CheckWalkCallbackFinish);
	
}

// Called every frame
void UHideSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TimelineCheckWall.TickTimeline(DeltaTime);
}

void UHideSystem::CheckWall()
{
	if(ActorOwner->XDirection!=0)
	{
		FHitResult RaycastResult1, RaycastResult2;
	
		GetWorld()->LineTraceSingleByChannel(RaycastResult1,  ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*ActorOwner->CapsuleHeight+ActorOwner->GetActorRightVector()*(ActorOwner->CapsuleRadius+OffsetRaycast)*ActorOwner->XDirection,  ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*ActorOwner->CapsuleHeight+ActorOwner->GetActorForwardVector()*50+ActorOwner->GetActorRightVector()*(ActorOwner->CapsuleRadius+OffsetRaycast)*ActorOwner->XDirection, ECC_GameTraceChannel3, ParamRaycast);
		//DrawDebugLine(GetWorld(),ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*ActorOwner->CapsuleHeight+ActorOwner->GetActorRightVector()*(ActorOwner->CapsuleRadius+OffsetRaycast)*ActorOwner->XDirection,  ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*ActorOwner->CapsuleHeight+ActorOwner->GetActorForwardVector()*50+ActorOwner->GetActorRightVector()*(ActorOwner->CapsuleRadius+OffsetRaycast)*ActorOwner->XDirection, FColor::Red, false, 0.5f,0,2);
	
		GetWorld()->LineTraceSingleByChannel(RaycastResult2,  ActorOwner->GetActorLocation()+ActorOwner->GetActorRightVector()*(ActorOwner->CapsuleRadius+OffsetRaycast)*ActorOwner->XDirection,  ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*50+ActorOwner->GetActorRightVector()*(ActorOwner->CapsuleRadius+OffsetRaycast)*ActorOwner->XDirection, ECC_GameTraceChannel3, ParamRaycast);
		//DrawDebugLine(GetWorld(),ActorOwner->GetActorLocation()+ActorOwner->GetActorRightVector()*(ActorOwner->CapsuleRadius+OffsetRaycast)*ActorOwner->XDirection,  ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*50+ActorOwner->GetActorRightVector()*(ActorOwner->CapsuleRadius+OffsetRaycast)*ActorOwner->XDirection, FColor::Red, false, 0.5f,0,2);

		bool Pivot = RaycastResult1.bBlockingHit && RaycastResult2.bBlockingHit;
	
		if(ActorOwner->CanMoveWhileHiding != Pivot)
		{
			if(!Pivot)
			{
				ActorOwner->GetCharacterMovement()->Velocity = FVector::Zero();
				ActorOwner->CameraStick->EnableAimCamPosition(ActorOwner->XDirection);
			}else
			{
				ActorOwner->CanMoveWhileHiding = true;
				ActorOwner->CameraStick->DisableAimCamPosition();
			}
		}
		
		ActorOwner->CanMoveWhileHiding = RaycastResult1.bBlockingHit && RaycastResult2.bBlockingHit;

		if(ActorOwner->CanMoveWhileHiding)
		{
			FRotator Pivot2 = FRotator(0,(RaycastResult2.ImpactNormal*-1).Rotation().Yaw,0);
			ActorOwner->SetActorRotation(Pivot2);
		}
	}

	TimelineCheckWall.PlayFromStart();
}

void UHideSystem::CheckHide()
{
	FHitResult RaycastResult1, RaycastResult2;
	//UE_LOG(LogTemp,Warning, TEXT("Checking For Hiding Place"));
	
	FVector LastNormal;
	FVector LastNormalDown;
	
	for (int i = -1; i<2; i++)
	{
		GetWorld()->LineTraceSingleByChannel(RaycastResult1,
			ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*ActorOwner->CapsuleHeight+ActorOwner->GetActorRightVector()*ActorOwner->CapsuleRadius*i,
			ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*ActorOwner->CapsuleHeight+ActorOwner->GetActorForwardVector()*50+ActorOwner->GetActorRightVector()*ActorOwner->CapsuleRadius*i,
			ECC_GameTraceChannel3, ParamRaycast);
		//DrawDebugLine(GetWorld(),GetActorLocation()+GetActorUpVector()*CapsuleHeight+GetActorRightVector()*CapsuleRadius*i, GetActorLocation()+GetActorUpVector()*CapsuleHeight+GetActorForwardVector()*50+GetActorRightVector()*CapsuleRadius*i, FColor::Red, false, 0.5f,0,2);
		
		if(i == -1)
		{
			LastNormal = RaycastResult1.ImpactNormal;
		}
		
		if(!RaycastResult1.bBlockingHit || LastNormal.Dot(RaycastResult1.ImpactNormal)<0.5f)
		{
			return;
		}

		//UE_LOG(LogTemp,Warning, TEXT("%s"), *RaycastResult1.HitObjectHandle.GetName());
		LastNormal=RaycastResult1.ImpactNormal;
	}

	for (int i = -1; i<2; i++)
	{
		GetWorld()->LineTraceSingleByChannel(RaycastResult2,  ActorOwner->GetActorLocation()+ActorOwner->GetActorRightVector()*ActorOwner->CapsuleRadius*i,
			ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*50+ActorOwner->GetActorRightVector()*ActorOwner->CapsuleRadius*i,
			ECC_GameTraceChannel3, ParamRaycast);
		//DrawDebugLine(GetWorld(),GetActorLocation()+GetActorRightVector()*CapsuleRadius*i, GetActorLocation()+GetActorForwardVector()*50+GetActorRightVector()*CapsuleRadius*i, FColor::Red, false, 0.5f,0,2);

		if(i == -1)
		{
			LastNormalDown = RaycastResult2.ImpactNormal;
		}
		
		if(!RaycastResult2.bBlockingHit || LastNormalDown.Dot(RaycastResult2.ImpactNormal)<0.5f){
			return;
		}

		LastNormalDown=RaycastResult2.ImpactNormal;
	}
	
	CurrentPlayerState->IsHiding = true;
	HideToWall(RaycastResult2.ImpactPoint,RaycastResult2.ImpactNormal);
	TimelineCheckWall.PlayFromStart();
}

void UHideSystem::CancelHide()
{
	TimelineCheckWall.Stop();
	
	ActorOwner->CanMoveWhileHiding = true;
	ActorOwner->CameraStick->DisableAimCamPosition();
	CurrentPlayerState->IsHiding = false;
	ActorOwner->GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
}

void UHideSystem::HideToWall(FVector ImpactPoint, FVector ImpactNormal)
{
	ActorOwner->CanMoveWhileHiding = true;
	ActorOwner->GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...
	FRotator Pivot = FRotator(0,(ImpactNormal*-1).Rotation().Yaw,0);
	ActorOwner->SetActorRotation(Pivot);
	ActorOwner->SetActorLocation(ImpactPoint-ActorOwner->GetActorRightVector()*ActorOwner->CapsuleRadius-ActorOwner->GetActorForwardVector()*ActorOwner->CapsuleRadius);
}


void UHideSystem::CheckCancelHide(FVector2d vec)
{
	const FRotator Rotation = CurrentController->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	float Test3 = round(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X).Dot(ActorOwner->GetActorRightVector()));
	float Test4 = round(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y).Dot(ActorOwner->GetActorRightVector()));
	
	if( FMath::Sign((Test3*vec.Y + Test4*vec.X)/2) == ActorOwner->XDirection)
	{
		CancelHide();
	}
}




