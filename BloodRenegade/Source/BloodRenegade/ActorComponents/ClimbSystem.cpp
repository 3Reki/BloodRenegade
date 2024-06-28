// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbSystem.h"
#include "BloodRenegade/Controller/BloodCharacter.h"
#include "BloodRenegade/Controller/BloodCharacterDataAsset.h"
#include "BloodRenegade/Controller/BloodPlayerState.h"
#include "BloodRenegade/WorldActors/CustomSpringArm.h"
#include "GameFramework/CharacterMovementComponent.h"

UClimbSystem::UClimbSystem()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UClimbSystem::BeginPlay()
{
	Super::BeginPlay();
	ActorOwner = GetOwner<ABloodCharacter>();
	CurrentPlayerState = GetOwner<ABloodCharacter>()->GetPlayerState<ABloodPlayerState>();
	ParamRaycast.AddIgnoredActor(ActorOwner.Get());

	FOnTimelineEventStatic CheckLedgeCallbackFinish;
	TimelineCheckLedge.SetTimelineLengthMode(TL_TimelineLength);
	TimelineCheckLedge.SetTimelineLength(0.01f);
	CheckLedgeCallbackFinish.BindUFunction(this, FName("CheckHighLedge"));
	TimelineCheckLedge.SetTimelineFinishedFunc( CheckLedgeCallbackFinish);
}

void UClimbSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TimelineCheckLedge.TickTimeline(DeltaTime);
}

bool UClimbSystem::CheckVault()
{
	AActor* Obstacle;
	FHitResult Result;
	GetWorld()->LineTraceSingleByChannel(Result,
			ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*(-ActorOwner->CapsuleHeight+DataAsset->VaultHeight/4),
			ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*(-ActorOwner->CapsuleHeight+DataAsset->VaultHeight/4)+ActorOwner->GetActorForwardVector()*70,
			ECC_WorldStatic, ParamRaycast);
	Obstacle = Result.HitObjectHandle.FetchActor();
	
	if(Result.bBlockingHit)
	{
		for (int i = 3; i>0; i--)
		{
			GetWorld()->LineTraceSingleByChannel(Result,
				ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*(-ActorOwner->CapsuleHeight+DataAsset->VaultHeight/i),
				ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*(-ActorOwner->CapsuleHeight+DataAsset->VaultHeight/i)+ActorOwner->GetActorForwardVector()*70,
				ECC_WorldStatic, ParamRaycast);
			
			if(!Result.bBlockingHit)
			{
				for (int j = 5; j>0; j--)
				{
					GetWorld()->LineTraceSingleByChannel(Result,
				ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*(-ActorOwner->CapsuleHeight+DataAsset->VaultHeight/i),
				ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*(-ActorOwner->CapsuleHeight+DataAsset->VaultHeight/i)+ActorOwner->GetActorForwardVector()*DataAsset->VaultLength/j,
					ECC_WorldStatic, ParamRaycast);
				

					if(!Result.bBlockingHit)
					{
						GetWorld()->LineTraceSingleByChannel(Result,
					ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*(-ActorOwner->CapsuleHeight+DataAsset->VaultHeight/i)+ActorOwner->GetActorForwardVector()*DataAsset->VaultLength/j,
					ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*(-ActorOwner->CapsuleHeight-20)+ActorOwner->GetActorForwardVector()*DataAsset->VaultLength/j,
						ECC_WorldStatic, ParamRaycast);
						
						if(Result.bBlockingHit && Result.HitObjectHandle.FetchActor()!=Obstacle)
						{
							ActorOwner->StartTranslateCharacter(ActorOwner->GetActorLocation(), Result.ImpactPoint + ActorOwner->GetActorUpVector()*ActorOwner->CapsuleHeight + ActorOwner->GetActorForwardVector()*ActorOwner->CapsuleRadius, DataAsset->VaultSpeed);
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	return false;
}

void UClimbSystem::CheckHighLedge()
{
	FHitResult RaycastResult4;

	for(float i = 0.6f; i<=1; i+=0.1f)
	{
		if(CheckLayer(i))
		{
			
			GetWorld()->LineTraceSingleByChannel(RaycastResult4,  ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*(ActorOwner->CapsuleHeight*0.70)+ActorOwner->GetActorForwardVector()*50,
		ActorOwner->GetActorLocation()+ActorOwner->GetActorForwardVector()*50,
			ECC_GameTraceChannel3, ParamRaycast);
	
			if(RaycastResult4.bBlockingHit)
			{
				ClimbWall(RaycastResult4.ImpactPoint);
				return;
			}
		}
	}

	TimelineCheckLedge.PlayFromStart();
	return;
}

bool UClimbSystem::CheckLayer(float high)
{
	FHitResult RaycastResult1,RaycastResult3;
	FVector LastNormal;
	
	for (int i = -1; i<=1; i++)
	{
		GetWorld()->LineTraceSingleByChannel(RaycastResult1,
			ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*ActorOwner->CapsuleHeight*0.85*high+ActorOwner->GetActorRightVector()*ActorOwner->CapsuleRadius*i,
			ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*ActorOwner->CapsuleHeight*0.85*high+ActorOwner->GetActorForwardVector()*70+ActorOwner->GetActorRightVector()*ActorOwner->CapsuleRadius*i,
			ECC_GameTraceChannel3, ParamRaycast);
		
		if(i == -1)
		{
			LastNormal = RaycastResult1.ImpactNormal;
		}
		
		if(!RaycastResult1.bBlockingHit || LastNormal.Dot(RaycastResult1.ImpactNormal)<0.5f || ActorOwner->GetActorUpVector().Dot(RaycastResult1.ImpactNormal) > 0.2)
		{
			return false;
		}

		LastNormal=RaycastResult1.ImpactNormal;
	}
	
	GetWorld()->LineTraceSingleByChannel(RaycastResult3,  ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*ActorOwner->CapsuleHeight,
		ActorOwner->GetActorLocation()+ActorOwner->GetActorUpVector()*ActorOwner->CapsuleHeight+ActorOwner->GetActorForwardVector()*70,
		ECC_GameTraceChannel3, ParamRaycast);
	
	if(RaycastResult3.bBlockingHit)
	{
		return false;
	}

	return true;
}

void UClimbSystem::ClimbWall(FVector impactPoint)
{
	ActorOwner->StartTranslateCharacter(ActorOwner->GetActorLocation(), impactPoint + ActorOwner->GetActorUpVector()*ActorOwner->CapsuleHeight,DataAsset->ClimbSpeed);
}

void UClimbSystem::CharacterVisible()
{
	ActorOwner->GetMesh()->SetVisibility(true);
	CurrentPlayerState->CurrentState = NormalState;
	ActorOwner->CameraStick->CameraLagSpeed = 10;
	ActorOwner->GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f); // ...at this rotation rate
	ActorOwner->CapsuleHeight = 90;
}