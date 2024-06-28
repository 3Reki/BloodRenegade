// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BloodRenegade/Interfaces/IInteractable.h"
#include "GameFramework/Actor.h"
#include "AriaEscort.generated.h"

class UCurveVector;
class UCapsuleComponent;
class USphereComponent;
class ABloodCharacter;
class UAriasDataAsset;

enum AriaState
{
	Reaching,
	Waiting,
	Duo,
};

UCLASS()
class BLOODRENEGADE_API AAriaEscort : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere)
	UAriasDataAsset* DataAsset;

	UPROPERTY(EditAnywhere)
	FVector WalkingOffset;

	UPROPERTY(EditAnywhere)
	float Speed = 2.0f;

	UFUNCTION(BlueprintCallable)
	void SetActiveEscort(ABloodCharacter* Player, bool b);

protected:
	UPROPERTY(EditAnywhere, Category=Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditAnywhere, Category=Colision, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleCollision;

	UPROPERTY(Blueprintable, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimInstance> CurrentAnims;

	AriaState CurrentState;
	FVector LastPosition;
	
	ABloodCharacter* Player;
	float ReachingLerp;
	
	AAriaEscort();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void TakeHandToIsera();
};
