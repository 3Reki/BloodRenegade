// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Blind.generated.h"

/**
 * 
 */

class ABloodPlayerState;
class UBloodCharacterDataAsset;
class ABloodHUD;
class ABloodCharacter;

UCLASS()
class BLOODRENEGADE_API UBlind : public UActorComponent
{
	GENERATED_BODY()

public:
	TWeakObjectPtr<ABloodHUD> HUD;
	TWeakObjectPtr<AActor> LastTarget;

	UFUNCTION()
	void UpdateUICurrentBlood(float Value);
	
	virtual void BeginPlay() override;
	virtual void Start();
	virtual void Invoke();
	void ResetSkill();
	void Cancel();
	void PreviewSkillTarget();
	void SetCurrentUIBlood(float f);
	void StartDecreaseBlood();
	void StopDecreaseBlood();
	void IncreaseBlood();
	void IncreaseUIBlood(float f);
	void DecreaseUIBlood(float f);

	UFUNCTION()
	void UnableFixBlood();
	
protected:
	UPROPERTY()
	FTimeline TimelineIncreaseBlood;

	UPROPERTY()
	FTimeline TimelineDecreaseBlood;
	
	UPROPERTY(EditAnywhere)
	UBloodCharacterDataAsset* DataAsset;
	
	FHitResult Result;
	FTimerHandle AddTargetTimerHandler, RecoverFirstBloodTimerHandler;
	TWeakObjectPtr<ABloodCharacter> Pawn;
	TWeakObjectPtr<ABloodPlayerState> CurrentPlayerState;
	FHitResult RaycastResult, RaycastResult2, RaycastResult3,RaycastResult4;
	FCollisionQueryParams CollisionParams;
	bool CanSkill, IsUsing, WasCrouch;
	int PreviewBloodGauge, CurrentUiBlood;
	FVector2D ScreenPos;
	FCollisionQueryParams ParamRaycast;
	FTimerHandle DecreasingHpTimerHandle, FirstBloodTimerHandle;
	
	void AddActorToTarget();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
