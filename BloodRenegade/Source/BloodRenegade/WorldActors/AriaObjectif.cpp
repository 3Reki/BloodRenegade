// Fill out your copyright notice in the Description page of Project Settings.


#include "AriaObjectif.h"

#include "AkGameplayStatics.h"
#include "AkGameplayTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "BloodRenegade/Controller/BloodCharacter.h"
#include "BloodRenegade/DataAssets/Sound/CharacterSoundDataAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SplineComponent.h"
#include "BloodRenegade/DataAssets/AriasDataAsset.h"

// Sets default values
AAriaObjectif::AAriaObjectif()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	CapsuleCollision->SetupAttachment(RootComponent);
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	SkeletalMesh->SetupAttachment(CapsuleCollision);

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	SplineComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AAriaObjectif::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentAnims = SkeletalMesh->GetAnimInstance();
	CurrentPoint = 0;

	MoveNextObjCurve = NewObject<UCurveFloat>();

	FOnTimelineEventStatic RevolutionCallBackStatic;
	FOnTimelineFloat RevolutionCallBack;
	RevolutionCallBack.BindUFunction(this, FName("MoveNextPoint"));
	RevolutionCallBackStatic.BindUFunction(this, FName("FinishedMovedPoint"));
	TimelineMoveNextPosition.AddInterpFloat(MoveNextObjCurve, RevolutionCallBack);
	TimelineMoveNextPosition.SetTimelineFinishedFunc(RevolutionCallBackStatic);
	
	SetActorLocation(SplineComponent->GetLocationAtTime(0, ESplineCoordinateSpace::World) + FVector::UpVector*60);
	SkeletalMesh->SetVisibility(false);
}

// Called every frame
void AAriaObjectif::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TimelineMoveNextPosition.TickTimeline(DeltaTime);

	if(IsDuoing)
	{
		if(CurrentPlayer->GetCharacterMovement()->Velocity.Size()>0.1f && !IsMoving)
		{
			IsMoving = true;
			CurrentAnims->ProcessEvent(CurrentAnims->FindFunction(FName("SetIsMoving")), &IsMoving);
		}else if(CurrentPlayer->GetCharacterMovement()->Velocity.Size()<=0.0f && IsMoving)
		{
			IsMoving = false;
			CurrentAnims->ProcessEvent(CurrentAnims->FindFunction(FName("SetIsMoving")), &IsMoving);
		}
		
		SetActorLocation(CurrentPlayer->GetActorLocation()+CurrentPlayer->GetActorRightVector()*WalkingOffset.X+CurrentPlayer->GetActorUpVector()*WalkingOffset.Z+CurrentPlayer->GetActorForwardVector()*WalkingOffset.Y);
		SetActorRotation(CurrentPlayer->GetActorRotation());
	}
}

void AAriaObjectif::NextObjectif()
{
	if(CurrentPoint>=SplineComponent->GetNumberOfSplinePoints()-1)
	{
		UAkGameplayStatics::SetState(nullptr, "Music_State", "InGame");
		return;
	}
	
	bool b = true;
	CurrentAnims->ProcessEvent(CurrentAnims->FindFunction(FName("SetIsMoving")), &b);
	MoveNextObjCurve->FloatCurve.UpdateOrAddKey(0,CurrentPoint/(SplineComponent->GetNumberOfSplinePoints()-1));
	CurrentPoint++;
	MoveNextObjCurve->FloatCurve.UpdateOrAddKey(DataAsset->SpeedMoveToNextObj,CurrentPoint/(SplineComponent->GetNumberOfSplinePoints()-1));
	TimelineMoveNextPosition.SetPlayRate(PointsActionTiming[CurrentPoint-1].SpeedToNextPoint);
	TimelineMoveNextPosition.PlayFromStart();
}

void AAriaObjectif::SetCurrentObj(int f)
{
	CurrentPoint = f;
	SetActorLocation(SplineComponent->GetLocationAtTime(CurrentPoint/(SplineComponent->GetNumberOfSplinePoints()-1), ESplineCoordinateSpace::World) + FVector::UpVector*60);
	SetActorRotation(SplineComponent->GetRotationAtTime(CurrentPoint/(SplineComponent->GetNumberOfSplinePoints()-1), ESplineCoordinateSpace::World));
	SkeletalMesh->SetVisibility(true);
}

void AAriaObjectif::StartObject()
{
	SkeletalMesh->SetVisibility(true);
	UAkGameplayStatics::SetState(nullptr, "Music_State", "AriaTheme");
	StartEmotionTimeline();
	NextObjectif();
}

float AAriaObjectif::GetTimingWithPoint()
{
	return PointsActionTiming[CurrentPoint].WaitingTimer;
}

void AAriaObjectif::SetActiveEscort(ABloodCharacter* Player, bool b)
{
	CurrentPlayer = Player;
	if(b)
	{
		IsDuoing = true;
		CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CurrentPlayer->SetActiveEscort(true);
	}else
	{
		IsDuoing = false;
		CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CurrentPlayer->SetActiveEscort(false);
	}
	
	CurrentAnims->ProcessEvent(CurrentAnims->FindFunction(FName("SetIsDuoing")), &IsDuoing);
}

void AAriaObjectif::MoveNextPoint(float Value)
{
	SetActorLocation(SplineComponent->GetLocationAtTime(Value, ESplineCoordinateSpace::World) + FVector::UpVector*60);
	SetActorRotation(FRotator(0, SplineComponent->GetRotationAtTime(Value, ESplineCoordinateSpace::World).Yaw, 0));
}

void AAriaObjectif::FinishedMovedPoint()
{
	bool b = false;
	CurrentAnims->ProcessEvent(CurrentAnims->FindFunction(FName("SetIsMoving")), &b);
	ExecutAction(PointsActionTiming[CurrentPoint].Action);
}

void AAriaObjectif::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	for(int j = 0; j<SplineComponent->GetNumberOfSplinePoints(); j++)
	{
		if(j>=PointsActionTiming.Num())
		{
			PointsActionTiming.Add(FActions(0,0));
		}
	}

	for(int j = PointsActionTiming.Num()-1; j>=0; j--)
	{
		if(j>=SplineComponent->GetNumberOfSplinePoints())
		{
			PointsActionTiming.RemoveAt(j);
		}
	}
}

