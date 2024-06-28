// Fill out your copyright notice in the Description page of Project Settings.


#include "AriaSentinel.h"

#include "BloodRenegade/ActorComponents/BiteSystem.h"
#include "BloodRenegade/ActorComponents/HookSystem.h"
#include "BloodRenegade/Controller/BloodCharacter.h"
#include "BloodRenegade/Controller/BloodPlayerState.h"
#include "BloodRenegade/DataAssets/AriasDataAsset.h"
#include "BloodRenegade/Widgets/W_AriasDetection.h"
#include "Components/SpotLightComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "BloodRenegade/Controller/BloodController.h"
#include "GameFramework/HUD.h"

// Sets default values
AAriaSentinel::AAriaSentinel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Collider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collider"));
	Collider->SetupAttachment(RootComponent);
	
	SkelletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	SkelletalMesh->SetupAttachment(Collider);

	LightSource = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	LightSource->SetupAttachment(Collider);

	MeshCollider = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Collider"));
	MeshCollider->SetupAttachment(LightSource);

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	SplineComponent->SetupAttachment(RootComponent);
}

void AAriaSentinel::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	float HypX, HypY, Dist;
	
	HypX = LightSource->AttenuationRadius * UKismetMathLibrary::Cos(FMath::DegreesToRadians(LightSource->InnerConeAngle));
	HypY = LightSource->AttenuationRadius * UKismetMathLibrary::Sin(FMath::DegreesToRadians(LightSource->InnerConeAngle));
	Dist = UKismetMathLibrary::Vector_Distance(LightSource->GetComponentLocation()+LightSource->GetRightVector()*LightSource->AttenuationRadius, LightSource->GetComponentLocation()+LightSource->GetRightVector()*HypX + LightSource->GetUpVector()*HypY);

	MeshCollider->SetRelativeLocation(FVector(HypX,0,0));
	MeshCollider->SetRelativeScale3D(FVector(Dist/52,Dist/52,HypX/100));

	for(int j = 0; j<SplineComponent->GetNumberOfSplinePoints(); j++)
	{
		if(j>=PointsTiming.Num())
		{
			PointsTiming.Add(0);
		}
	}

	for(int j = PointsTiming.Num()-1; j>=0; j--)
	{
		if(j>=SplineComponent->GetNumberOfSplinePoints())
		{
			PointsTiming.RemoveAt(j);
		}
	}
}

// Called when the game starts or when spawned
void AAriaSentinel::BeginPlay()
{
	Super::BeginPlay();
	
	ParamRaycast.AddIgnoredActor(this);
	NextPoint = 1;
	FRotator rotation = UKismetMathLibrary::FindLookAtRotation(LightSource->GetComponentLocation(), SplineComponent->GetLocationAtTime(0, ESplineCoordinateSpace::World));
	LightSource->SetWorldRotation(rotation);

	ComebackPatrolCurve = NewObject<UCurveFloat>();
	ComebackPatrolCurve->FloatCurve.UpdateOrAddKey(0,0);
	ComebackPatrolCurve->FloatCurve.UpdateOrAddKey(DataAsset->SearchingSpeed,1);

	FOnTimelineFloat ComeBackCallBack;
	FOnTimelineEventStatic ComeBackCallBackStatic;
	
	ComeBackCallBack.BindUFunction(this, FName("RotateTowardLastPoint"));
	TimelineComeBackPatrol.AddInterpFloat(ComebackPatrolCurve, ComeBackCallBack);
	
	ComeBackCallBackStatic.BindUFunction(this, FName("StartTimelineCheck"));
	TimelineComeBackPatrol.SetTimelineFinishedFunc(ComeBackCallBackStatic);
		
	RevolutionCurve = NewObject<UCurveFloat>();
	RevolutionCurve->FloatCurve.AddKey(0,0);
	RevolutionCurve->FloatCurve.AddKey(DataAsset->SearchingSpeed,NextPoint/(SplineComponent->GetNumberOfSplinePoints()-1));

	FOnTimelineFloat RevolutionCallBack;
	FOnTimelineEventStatic RevolutionCallBackStatic;
	
	RevolutionCallBack.BindUFunction(this, FName("RotateTowardPoint"));
	TimelineForRevolution.AddInterpFloat(RevolutionCurve, RevolutionCallBack);
	
	RevolutionCallBackStatic.BindUFunction(this, FName("Next"));
	TimelineForRevolution.SetTimelineFinishedFunc(RevolutionCallBackStatic);

	GetWorld()->GetTimerManager().ClearTimer(WaitForNextPointTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(WaitForNextPointTimerHandle, this, &AAriaSentinel::StartTimelineCheck, PointsTiming[0], false);

	CurrentGaugeValue = 0;
}

// Called every frame
void AAriaSentinel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimelineForRevolution.TickTimeline(DeltaTime);
	TimelineComeBackPatrol.TickTimeline(DeltaTime);
	
	if(InCollider && Player.Get())
	{
		GetWorld()->LineTraceSingleByChannel(RaycastResult,  LightSource->GetComponentLocation(),  Player->GetActorLocation(),  ECC_GameTraceChannel3, ParamRaycast);
		if(RaycastResult.HitObjectHandle.FetchActor() == Player)
		{
			if(!SeeThePlayer)
			{
				SeeThePlayer = true;
				GetWorld()->GetTimerManager().ClearTimer(WaitForNextPointTimerHandle);
				TimelineForRevolution.Stop();
			}
			
			if(!Player->CurrentPlayerState->IsDashing && !Player->HookSystemNew->IsHooking)
			{
				FRotator rotation = UKismetMathLibrary::FindLookAtRotation(LightSource->GetComponentLocation(), Player->GetActorLocation());
				LightSource->SetWorldRotation(rotation);

				if(Player->BiteSystem->IsSucking && !IsBitingPivot)
				{
					GetWorldTimerManager().ClearTimer(IncreaseGaugeTimerHandle);
					GetWorldTimerManager().ClearTimer(WaitForDecreaseTimerHandle);
					GetWorldTimerManager().ClearTimer(DecreaseGaugeTimerHandle);
					IsBitingPivot = true;
					GaugeDecreasingPivot = false;
					
					GetWorldTimerManager().SetTimer(IncreaseGaugeTimerHandle, this, &AAriaSentinel::IncreaseGauge, 0.033f, true);
				}else if(!Player->BiteSystem->IsSucking && IsBitingPivot)
				{
					GetWorldTimerManager().ClearTimer(IncreaseGaugeTimerHandle);
					GetWorldTimerManager().ClearTimer(DecreaseGaugeTimerHandle);
					GetWorldTimerManager().ClearTimer(WaitForDecreaseTimerHandle);
					IsBitingPivot = false;
					GaugeDecreasingPivot = true;
					
					GetWorldTimerManager().SetTimer(WaitForDecreaseTimerHandle, this, &AAriaSentinel::StartDecreasing, DataAsset->TimingWaitForDecreaseGauge, false);
				}
			}
		}else
		{
			if(SeeThePlayer)
			{
				SeeThePlayer = false;
				IsBitingPivot = false;
				GetWorldTimerManager().ClearTimer(IncreaseGaugeTimerHandle);
				GetWorldTimerManager().SetTimer(WaitForPatrolAgainTimerHandle, this, &AAriaSentinel::StartTimelineComeBackPatrol, 1,false);

				if(!GaugeDecreasingPivot)
				{
					GaugeDecreasingPivot = true;
					GetWorldTimerManager().SetTimer(WaitForDecreaseTimerHandle, this, &AAriaSentinel::StartDecreasing, DataAsset->TimingWaitForDecreaseGauge, false);
				}
			}
		}
	}
}

void AAriaSentinel::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	if(Cast<ABloodCharacter>(OtherActor))
	{
		LastPosition = SplineComponent->GetLocationAtTime(RevolutionCurve->FloatCurve.Eval(TimelineForRevolution.GetPlaybackPosition()), ESplineCoordinateSpace::World);
		GetWorldTimerManager().ClearTimer(WaitForPatrolAgainTimerHandle);
		Player = Cast<ABloodCharacter>(OtherActor);
		SetActiveHUD(true);
		InCollider =true;
	}
}

void AAriaSentinel::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if(Cast<ABloodCharacter>(OtherActor))
	{
		SetActiveHUD(false);
		SeeThePlayer = false;
		InCollider = false;
		IsBitingPivot = false;
		GetWorldTimerManager().ClearTimer(IncreaseGaugeTimerHandle);
		GetWorldTimerManager().SetTimer(WaitForPatrolAgainTimerHandle, this, &AAriaSentinel::StartTimelineComeBackPatrol, 1,false);
		if(!GaugeDecreasingPivot)
		{
			GaugeDecreasingPivot = true;
			GetWorldTimerManager().SetTimer(WaitForDecreaseTimerHandle, this, &AAriaSentinel::StartDecreasing, DataAsset->TimingWaitForDecreaseGauge, false);
		}

		Player = nullptr;
	}
}

void AAriaSentinel::RotateTowardPoint(float Value)
{
	FRotator rotation = UKismetMathLibrary::FindLookAtRotation(LightSource->GetComponentLocation(), SplineComponent->GetLocationAtTime(Value, ESplineCoordinateSpace::World));
	LightSource->SetWorldRotation(rotation);
}


void AAriaSentinel::Next()
{
	GetWorld()->GetTimerManager().ClearTimer(WaitForNextPointTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(WaitForNextPointTimerHandle, this, &AAriaSentinel::StartTimelineCheck, PointsTiming[NextPoint], false);
	
	RevolutionCurve->FloatCurve.UpdateOrAddKey(0,NextPoint/(SplineComponent->GetNumberOfSplinePoints()-1));
	
	if(NextPoint == SplineComponent->GetNumberOfSplinePoints()-1 && !Reverse)
	{
		Reverse = true;
	}else if(NextPoint == 0 && Reverse)
	{
		Reverse = false;
	}
	
	if(Reverse)
	{
		NextPoint--;
	}else
	{
		NextPoint++;
	}
	
	RevolutionCurve->FloatCurve.UpdateOrAddKey(DataAsset->SearchingSpeed,NextPoint/(SplineComponent->GetNumberOfSplinePoints()-1));
	TimelineForRevolution.SetNewTime(0);
}

void AAriaSentinel::RotateTowardLastPoint(float Value)
{
	FRotator rotation = UKismetMathLibrary::FindLookAtRotation(LightSource->GetComponentLocation(), LastPosition);
	LightSource->SetWorldRotation(FMath::Lerp(LastRotation, rotation, Value));
}

void AAriaSentinel::StartTimelineCheck()
{
	TimelineForRevolution.Play();
}

void AAriaSentinel::StartDecreasing()
{
	GetWorldTimerManager().SetTimer(DecreaseGaugeTimerHandle, this, &AAriaSentinel::DecreaseGauge, 0.03f, true);
}

void AAriaSentinel::StartTimelineComeBackPatrol()
{
	LastRotation = LightSource->GetComponentRotation();
	TimelineComeBackPatrol.PlayFromStart();
}

void AAriaSentinel::IncreaseGauge()
{
	float pivot = CurrentGaugeValue+1;	
	if(pivot>DataAsset->MaxGauge)
	{
		GetWorldTimerManager().ClearTimer(IncreaseGaugeTimerHandle);
		Player->TakeHit(100);
	}

	CurrentGaugeValue = pivot;
	DetectionHUD->SetPercent(CurrentGaugeValue/DataAsset->MaxGauge);
}

void AAriaSentinel::DecreaseGauge()
{
	int pivot = CurrentGaugeValue-1;
	if(pivot<0)
	{
		GetWorldTimerManager().ClearTimer(DecreaseGaugeTimerHandle);
		pivot = 0;
	}

	CurrentGaugeValue = pivot;
	DetectionHUD->SetPercent(CurrentGaugeValue/DataAsset->MaxGauge);
}

void AAriaSentinel::SetActiveHUD(bool b)
{
	if(b)
	{
		if(!DetectionHUD)
		{
			DetectionHUD = CreateWidget<UW_AriasDetection>(Player->GetController<ABloodController>(), WidgetAriaDetectionClass);
			DetectionHUD->AddToViewport();
		}else
		{
			DetectionHUD->SetRenderOpacity(1);
		}
	}else
	{
		DetectionHUD->SetRenderOpacity(0);
	}
}
