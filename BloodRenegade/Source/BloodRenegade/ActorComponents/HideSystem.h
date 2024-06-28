// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "HideSystem.generated.h"


class ABloodController;
class ABloodHUD;
class ABloodPlayerState;
class ABloodCharacter;
class UBloodCharacterDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOODRENEGADE_API UHideSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void CheckHide();
	
	UHideSystem();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void CancelHide();
	void HideToWall(FVector ImpactPoint, FVector ImpactNormal);
	void CheckCancelHide(FVector2d vec);
	
	
	UPROPERTY()
	FTimeline  TimelineCheckWall;
protected:
	const int OffsetRaycast = 30;
	FCollisionQueryParams ParamRaycast;
	TWeakObjectPtr<ABloodCharacter> ActorOwner;
	TWeakObjectPtr<ABloodPlayerState> CurrentPlayerState;
	TWeakObjectPtr<APlayerCameraManager> PlayerCameraManager;
	TWeakObjectPtr<ABloodController> CurrentController;
	FTimerHandle HideTimerHandler;
	UFUNCTION()
	void CheckWall();

	virtual void BeginPlay() override;
};
