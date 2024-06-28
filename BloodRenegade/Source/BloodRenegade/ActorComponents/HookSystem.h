// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "HookSystem.generated.h"

class UCurveVector;
class ABloodHUD;
class ABloodController;
class ABloodPlayerState;
class ABloodCharacter;
class UBloodCharacterDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOODRENEGADE_API UHookSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	bool IsInSolarLight;
	
	UHookSystem();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Hook();
	bool IsHooking, ResetCamDone;
	void ResetCamHook();
	void FinishHook();
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBloodCharacterDataAsset* DataAsset;

	UPROPERTY()
	FTimeline ResetCamTimeline;
	
	FHitResult RaycastResult;
	FCollisionQueryParams ParamRaycast;
	TWeakObjectPtr<ABloodCharacter> ActorOwner;
	TWeakObjectPtr<ABloodPlayerState> CurrentPlayerState;
	TWeakObjectPtr<ABloodController> CurrentController;
	TWeakObjectPtr<ABloodHUD> CurrentHud;
	FRotator LastDireRecord;
	bool CanHook;
	FTimerHandle EnableTimerHandle, FinishHookTimerHandle;
	UCurveFloat* ResetCamCurv;
	FRotator LastCamRotation, NextCamRotation;

	UFUNCTION()
	void LerpResetCamera(float val);
	
	virtual void BeginPlay() override;
	TWeakObjectPtr<AActor>CheckPointClosest(TArray<FHitResult> Results);
	void CheckHookPoint();
	void EnableHook();
};
