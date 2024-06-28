// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BloodPlayerState.generated.h"

/**
 * 
 */

enum States
{
	CantMoveAndRotate,
	CantMove,
	CanOnlyMove,
	NormalState,
	PowerOff,
};

UCLASS()
class BLOODRENEGADE_API ABloodPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsBitting;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsCrouched;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsHiding;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsStunned;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsOnLedge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsDashing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsEscorting;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsInNightmareForm;

	bool IsAiming;
	TWeakObjectPtr<AActor> ObjectInteractable, CurrentEnnemi, HookTarget;

	TArray<TWeakObjectPtr<AActor>> CurrentTargetEnemy;
	
	States CurrentState;

	int CurrentHP;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentBloodGauge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int KillCounter;
};
