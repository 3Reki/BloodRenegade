// Fill out your copyright notice in the Description page of Project Settings.


#include "AriaEscort.h"
#include "BloodRenegade/Controller/BloodCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAriaEscort::AAriaEscort()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	CapsuleCollision->SetupAttachment(RootComponent);
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh Component"));
	SkeletalMesh->SetupAttachment(CapsuleCollision);
}

// Called when the game starts or when spawned
void AAriaEscort::BeginPlay()
{
	Super::BeginPlay();
	CurrentAnims = SkeletalMesh->GetAnimInstance();
	CurrentState = Waiting;
}

// Called every frame
void AAriaEscort::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FRotator rotateTo;
	switch (CurrentState)
	{
		case Reaching:
			SetActorLocation(FMath::Lerp(LastPosition, Player->GetActorLocation()+Player->GetActorRightVector()*WalkingOffset.X+Player->GetActorUpVector()*WalkingOffset.Z+Player->GetActorForwardVector()*WalkingOffset.Y, ReachingLerp));
			ReachingLerp+=Speed*DeltaTime;
			//rotateTo = UKismetMathLibrary::FindLookAtRotation(Player->GetActorLocation(), GetActorLocation());
			//Player->GetController()->ClientSetRotation(rotateTo);
			if(FVector::Distance(GetActorLocation(), Player->GetActorLocation()+Player->GetActorRightVector()*WalkingOffset.X+Player->GetActorUpVector()*WalkingOffset.Z+Player->GetActorForwardVector()*WalkingOffset.Y)<=80.0f)
			{
				TakeHandToIsera();
			}
			break;
		case Duo:
			SetActorLocation(Player->GetActorLocation()+Player->GetActorRightVector()*WalkingOffset.X+Player->GetActorUpVector()*WalkingOffset.Z+Player->GetActorForwardVector()*WalkingOffset.Y);
			SetActorRotation(Player->GetActorRotation());
			break;
	}
}

void AAriaEscort::TakeHandToIsera()
{
	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CurrentState = Duo;
	Player->SetActiveEscort(true);
}

void AAriaEscort::SetActiveEscort(ABloodCharacter* player,bool b)
{
	if(b)
	{
		Player = player;
		ReachingLerp = 0;
		CurrentState = Reaching;
		LastPosition = GetActorLocation();
	}else
	{
		CurrentState = Waiting;
		CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Player->SetActiveEscort(false);
	}
}

