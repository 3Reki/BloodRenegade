// Fill out your copyright notice in the Description page of Project Settings.


#include "VampiricVisionComponent.h"
#include "AkGameplayStatics.h"
#include "BloodRenegade/AI/Soldier.h"
#include "BloodRenegade/Controller/BloodCharacter.h"
#include "BloodRenegade/Controller/BloodCharacterDataAsset.h"
#include "BloodRenegade/Controller/BloodController.h"
#include "BloodRenegade/Controller/BloodPlayerState.h"
#include "BloodRenegade/Controller/BloodHUD.h"
#include "BloodRenegade/DataAssets/Sound/CharacterSoundDataAsset.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/CharacterMovementComponent.h"
// Sets default values for this component's properties
UVampiricVisionComponent::UVampiricVisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVampiricVisionComponent::BeginPlay()
{
	Super::BeginPlay();
	ActorOwner = GetOwner<ABloodCharacter>();
	CurrentPlayerState = GetOwner<ABloodCharacter>()->GetPlayerState<ABloodPlayerState>();
	ParamRaycast.AddIgnoredActor(ActorOwner.Get());
	CurrentHUD = ActorOwner->GetController<ABloodController>()->GetHUD<ABloodHUD>();
	PlayerCameraManager = ActorOwner->GetController<ABloodController>()->PlayerCameraManager;
	CurrentController = ActorOwner->GetController<ABloodController>();
	SphereVisionVampiric = ActorOwner->GetWorld()->SpawnActor<AStaticMeshActor>(DataAsset->SphereMesh, FVector::Zero(), FRotator::ZeroRotator);
	SphereVisionVampiric->SetActorScale3D(FVector::One()*(DataAsset->RadiusForVampiricVision/50));
	CanUseVV = true;
	// ...
}

// Called every frame
void UVampiricVisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UVampiricVisionComponent::DetectEnemy()
{
	ActorOwner->EffectScanVampiricVision();

	Shape = FCollisionShape::MakeSphere(DataAsset->RadiusForVampiricVision);
	GetWorld()->SweepMultiByChannel(RaycastResult2, ActorOwner->GetActorLocation(), ActorOwner->GetActorLocation(), ActorOwner->GetActorRotation().Quaternion(), ECC_GameTraceChannel1, Shape, ParamRaycast);

	for (auto hits : RaycastResult2)
	{
		if(hits.HitObjectHandle.DoesRepresentClass(ASoldier::StaticClass()))
		{
			Cast<ASoldier>(hits.GetActor())->StartVampiricVisible(DataAsset->TimerForVampiricVision);
		}
	}
}


void UVampiricVisionComponent::StartVision()
{
	if(!IsVisionActif && CanUseVV)
	{
		IsVisionActif = true;
		ActorOwner->StartVampiricVisionEffect();
		DetectEnemy();
		ActorOwner->GetWorldTimerManager().SetTimer(VampiricTimerHandle, this, &UVampiricVisionComponent::DetectEnemy, DataAsset->TimerForVampiricVision, true);
		SphereVisionVampiric->SetActorLocation(ActorOwner->GetActorLocation());
		const FOnAkPostEventCallback NullCallback;
		UAkGameplayStatics::PostEvent(ActorOwner->SoundDataAsset->VampiricSenseFadeInAudioEvent, ActorOwner.Get(), 0, NullCallback);
	}
}

void UVampiricVisionComponent::StopVision()
{
	if(IsVisionActif)
	{
		ActorOwner->GetWorldTimerManager().ClearTimer(VampiricTimerHandle);
		IsVisionActif = false;
		ActorOwner->CancelVampiricVisionEffect();
		const FOnAkPostEventCallback NullCallback;
		UAkGameplayStatics::PostEvent(ActorOwner->SoundDataAsset->VampiricSenseFadeOutAudioEvent, ActorOwner.Get(), 0, NullCallback);
	}
}

void UVampiricVisionComponent::SwitchVision()
{
	ActorOwner->GetWorldTimerManager().ClearTimer(VampiricTimerHandle);

	if(!IsVisionActif)
	{
		StartVision();
		return;
	}

	StopVision();
}

