// Fill out your copyright notice in the Description page of Project Settings.


#include "LampUV.h"

#include "AkGameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "BloodRenegade/Widgets/W_BlindMarker.h"
#include "LampUVDataAsset.h"
#include "BloodRenegade/ActorComponents/HookSystem.h"
#include "BloodRenegade/Controller/BloodCharacter.h"
#include "BloodRenegade/Controller/BloodController.h"
#include "BloodRenegade/Controller/BloodPlayerState.h"
#include "BloodRenegade/Controller/HealthSystem.h"
#include "BloodRenegade/DataAssets/Sound/CharacterSoundDataAsset.h"
#include "BloodRenegade/Skills/Blind.h"
#include "Kismet/KismetMathLibrary.h"
#include "Perception/AISense_Hearing.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ALampUV::ALampUV()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshSpot"));
	BaseMesh->SetupAttachment(RootComponent);
	
	LightSource = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	LightSource->SetupAttachment(BaseMesh);

	MeshCollider = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MEsh"));
	MeshCollider->SetupAttachment(LightSource);

	SoftLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SoftLight"));
	SoftLight->SetupAttachment(LightSource);

	Flare1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Flare 1"));
	Flare1->SetupAttachment(LightSource);

	Flare2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Flare 2"));
	Flare2->SetupAttachment(LightSource);
	
	TeamID = 2;
}


void ALampUV::OnConstruction(const FTransform& Transform)
{
	float HypX, HypY, Dist;
	
	HypX = LightSource->AttenuationRadius * UKismetMathLibrary::Cos(FMath::DegreesToRadians(LightSource->InnerConeAngle));
	HypY = LightSource->AttenuationRadius * UKismetMathLibrary::Sin(FMath::DegreesToRadians(LightSource->InnerConeAngle));
	Dist = UKismetMathLibrary::Vector_Distance(LightSource->GetComponentLocation()+LightSource->GetRightVector()*LightSource->AttenuationRadius, LightSource->GetComponentLocation()+LightSource->GetRightVector()*HypX + LightSource->GetUpVector()*HypY);

	MeshCollider->SetRelativeLocation(FVector(HypX,0,0));
	MeshCollider->SetRelativeScale3D(FVector(Dist/52,Dist/52,HypX/100));
}

FGenericTeamId ALampUV::GetGenericTeamId() const
{
	return TeamID;
}


void ALampUV::BeingBlind_Implementation(float f)
{
	IIBlindable::BeingBlind_Implementation(f);
	BlindEffect(true);
	SwitchOff();
	GetWorldTimerManager().SetTimer(SwitchOffTimer, this, &ALampUV::SwitchOn, DataAsset->BlindTimer, false);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetActorLocation(), 1, this, DataAsset->SoundRange, FName(TEXT("Instant")));
}

void ALampUV::BeginPlay()
{
	Super::BeginPlay();
	ParamRaycast.AddIgnoredActor(this);
	
}


void ALampUV::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if(Cast<ABloodCharacter>(OtherActor))
	{
		CurrentPlayer = Cast<ABloodCharacter>(OtherActor);
		GetWorld()->LineTraceSingleByChannel(RaycastResult,  LightSource->GetComponentLocation(),  CurrentPlayer->GetActorLocation(),  ECC_WorldStatic, ParamRaycast);

		if(RaycastResult.HitObjectHandle.FetchActor() == CurrentPlayer && !CurrentPlayer->CurrentPlayerState->IsInNightmareForm)
		{
			InLight();
		}
		
		InCollider = true;
	}
}

void ALampUV::NotifyActorEndOverlap(AActor* OtherActor)
{
	if(OtherActor == CurrentPlayer)
	{
		InCollider = false;
		OutLight();
		CurrentPlayer = nullptr;
		CurrentController = nullptr;
	}
}

void ALampUV::InLight()
{
	CurrentPlayer->HookSystemNew->IsInSolarLight = true;
	CurrentPlayer->GetWorldTimerManager().ClearTimer(DecreaseStaminaTimerHandle);
	CurrentPlayer->SetModifier(DataAsset->CharacterSpeedModifier);
	CurrentController = CurrentPlayer->GetController<ABloodController>();
	CameraShake = CurrentController->PlayerCameraManager->StartCameraShake(DataAsset->CameraShakeLight, 1);
	CurrentController->BlindSkill->StartDecreaseBlood();
	CurrentController->VibrationInLight(true);
	
	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(CurrentPlayer->SoundDataAsset->SolarBurnEvent, CurrentPlayer.Get(), 0, NullCallback);
	
	CurrentPlayer->InLightEffect(true);
	TWeakObjectPtr<ABloodCharacter> WeakThis = CurrentPlayer;

	GetWorldTimerManager().SetTimer(TakeDamage, FTimerDelegate::CreateWeakLambda(this, 
		   [WeakThis]
		   {
			   if(WeakThis.IsValid())
			   {
				   WeakThis->TakeHit(1);
			   }
		   }), DataAsset->TakeDamageTimer, true);
}

void ALampUV::OutLight()
{
	if(!CurrentPlayer.Get() || !CurrentController.Get()) return;

	GetWorldTimerManager().ClearTimer(TakeDamage);
	CurrentController->VibrationInLight(false);
	CurrentController->PlayerCameraManager->StopAllCameraShakes();
	CurrentPlayer->HookSystemNew->IsInSolarLight = false;
	CurrentPlayer->GetWorldTimerManager().ClearTimer(DecreaseStaminaTimerHandle);
	CurrentPlayer->SetModifier(1);
	CurrentPlayer->HealthSystem->WaitForResetHP(DataAsset->WaitTimerResetHp);
	CurrentPlayer->InLightEffect(false);
	CurrentController->BlindSkill->StopDecreaseBlood();

	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(CurrentPlayer->SoundDataAsset->SolarBurnStopEvent, CurrentPlayer.Get(), 0, NullCallback);
}

// Called every frame
void ALampUV::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(InCollider && CurrentPlayer.Get())
	{
		GetWorld()->LineTraceSingleByChannel(RaycastResult,  LightSource->GetComponentLocation(),  CurrentPlayer->GetActorLocation(), ECC_WorldStatic, ParamRaycast);
		if(RaycastResult.HitObjectHandle.FetchActor() == CurrentPlayer && !CurrentPlayer->CurrentPlayerState->IsInNightmareForm)
		{
			if(!IsVisible)
			{
				IsVisible = true;
				InLight();
			}
		}else
		{
			IsVisible = false;
			OutLight();
		}
	}
}

