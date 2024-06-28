// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "AriaSentinel.generated.h"

class UW_AriasDetection;
class UAriasDataAsset;
class ABloodCharacter;
class USplineComponent;
class USpotLightComponent;
class UCapsuleComponent;

UCLASS()
class BLOODRENEGADE_API AAriaSentinel : public AActor
{
	GENERATED_BODY()
	
public:
	AAriaSentinel();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UAriasDataAsset* DataAsset;
	
protected:
	UPROPERTY(EditAnywhere, Category=Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> SkelletalMesh;

	UPROPERTY(EditAnywhere, Category=Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> Collider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* LightSource;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshCollider;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<float> PointsTiming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Widget)
	TSubclassOf<UW_AriasDetection> WidgetAriaDetectionClass;

	UPROPERTY()
	UW_AriasDetection* DetectionHUD;

	UPROPERTY()
	FTimeline TimelineForRevolution;

	UPROPERTY()
	FTimeline TimelineComeBackPatrol;

	TWeakObjectPtr<ABloodCharacter> Player;
	FTimerHandle WaitForNextPointTimerHandle, WaitForPatrolAgainTimerHandle, DecreaseGaugeTimerHandle, IncreaseGaugeTimerHandle, WaitForDecreaseTimerHandle;
	UCurveFloat* RevolutionCurve, *ComebackPatrolCurve;
	float NextPoint;
	bool Reverse;
	FHitResult RaycastResult;
	FCollisionQueryParams ParamRaycast;
	FVector LastPosition;
	float CurrentGaugeValue;
	bool InCollider, IsBitingPivot, GaugeDecreasingPivot, SeeThePlayer;
	FRotator LastRotation;
	
	UFUNCTION()
	void RotateTowardPoint(float Value);

	UFUNCTION()
	void RotateTowardLastPoint(float Value);

	UFUNCTION()
	void Next();

	UFUNCTION()
	void StartTimelineCheck();

	void StartDecreasing();
	void StartTimelineComeBackPatrol();
	void IncreaseGauge();
	void DecreaseGauge();
	void SetActiveHUD(bool b);

	
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
