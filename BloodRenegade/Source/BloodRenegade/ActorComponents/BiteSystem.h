// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "BiteSystem.generated.h"

class UVibationDataAsset;
class ABloodController;
class ABloodHUD;
class ABloodPlayerState;
class ABloodCharacter;
class UBloodCharacterDataAsset;

enum BiteType
{
	OnGround,
	InAir,
	OnLedge,
	NightmareForm,
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOODRENEGADE_API UBiteSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UBloodCharacterDataAsset> DataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UVibationDataAsset> VibroDataAsset;

	bool IsSucking, IsTraveling, PivotIsAttacking, IsNearForQTE, IsBiting, IsRangeVisible, FirstCheckNearQTE;
	
	UFUNCTION()
	void EndBite();

	UFUNCTION(BlueprintCallable)
	void SetAnimLedgeBite(bool b);

	UFUNCTION(BlueprintCallable)
	void ShortBite();

	UFUNCTION(BlueprintCallable)
	void FinisherLongBite();

	UFUNCTION(BlueprintCallable)
	void OnGroundBite();

	UFUNCTION(BlueprintCallable)
	void OnLedgeBite();

	UFUNCTION(BlueprintCallable)
	void ToggleShowBiteRange();

	UFUNCTION(BlueprintCallable)
	void UnfreezePlayer();
	
	UBiteSystem();

	void CheckInputQTE();
	void ExecuteQTE();
	void StartAttacking();
	void StartBiting();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetCurrentBiteType(BiteType type);
	int EnemiEndAlive;
protected:
	UPROPERTY()
	FTimeline TimelineWaitForBite;

	UPROPERTY()
	FTimeline TimelineQTE;
	
	FHitResult RaycastResult;

	TMap<BiteType, float> RangeQTEBites;
	FCollisionQueryParams ParamRaycast;
	TWeakObjectPtr<ABloodCharacter> ActorOwner;
	TWeakObjectPtr<ABloodPlayerState> CurrentPlayerState;
	TWeakObjectPtr<ABloodHUD> CurrentHUD;
	TWeakObjectPtr<APlayerCameraManager> PlayerCameraManager;
	TWeakObjectPtr<ABloodController> CurrentController;
	FVector2d OnScreenLoc;
	bool WasCrouched, IsFinishing, CanCheckForEnemy, StartedQTE, IsQTETiming, FinishedQTE;
	FTimerHandle FinisherTimerHandle, CheckBiteTimerHandle, PivotAttackingTimerHandle;
	BiteType CurrentBiteType, LastType;
	FCollisionShape Shape;
	float Percent;
	
	UFUNCTION()
	void UpdateBiteHoldHUD(float Value);

	UFUNCTION()
	void FinishQTE();

	UFUNCTION()
	void UpdateTimingQTE(float Value);

	UFUNCTION(BlueprintCallable)
	void PlayBiteSound();
	
	virtual void BeginPlay() override;
	void LongBite();
	void ExecuteBite();
	void CheckEnemyForBite();
	void StartQTE();
	void StopQTE();
};
