// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillIndicator.h"

// Sets default values
ASkillIndicator::ASkillIndicator()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
}

// Called when the game starts or when spawned
void ASkillIndicator::BeginPlay()
{
	Super::BeginPlay();
}

void ASkillIndicator::Move(FVector v)
{
	SetActorLocation(v);
	CheckWall();
}


void ASkillIndicator::Show(bool b)
{
	if(IsActive==b) return;
	IsActive = b;
	
	if(IsActive)
	{
		EventFX.ExecuteIfBound(true);
	}else
	{
		EventFX.ExecuteIfBound(false);
	}
}

void ASkillIndicator::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}


bool ASkillIndicator::GetEnoughSpace()
{
	EventNotSpaceFX.ExecuteIfBound(EnoughtPlace);
	return EnoughtPlace;
}

void ASkillIndicator::ChangeFX(int index)
{
	EventFX.Unbind();
	EventNotSpaceFX.Unbind();
	
	switch (index)
	{
		case 0:
			EventFX.BindUFunction(this, "DecoyEffect", NULL);
			EventNotSpaceFX.BindUFunction(this, "NotSpaceDecoyEffect", NULL);
			break;
		case 1:
			EventFX.BindUFunction(this, "BlindEffect", NULL);
			break;
		default:
			break;
	}
}

void ASkillIndicator::CheckWall()
{
	FHitResult _raycastResult;
	FCollisionQueryParams _collisionParams;
	_collisionParams.AddIgnoredActor(this);
	
	FCollisionShape _capsule = FCollisionShape::MakeCapsule(24.0f, 85);
	GetWorld()->SweepSingleByChannel(_raycastResult, GetActorLocation(), GetActorLocation(), GetActorRotation().Quaternion(),ECC_Pawn, _capsule, _collisionParams);
	//DrawDebugCapsule(GetWorld(), GetActorLocation(), 85, 24,GetActorRotation().Quaternion(), FColor::Blue, false, 1.0f);
	if(_raycastResult.bBlockingHit)
	{
		EnoughtPlace = false;
		return;
	}

	EnoughtPlace = true;
}





