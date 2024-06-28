// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "../DataAssets/SoldierDataAsset.h"
#include "WeaponComponent.generated.h"

class UCurveFloat;
class UNiagaraSystem;
struct FTimerHandle;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOODRENEGADE_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere)
	USoldierDataAsset* SoldierDA;

	// Sets default values for this component's properties
	UWeaponComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	USceneComponent* MuzzlePoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UMeshComponent* WeaponMesh;

	virtual void Fire(FVector direction, FVector startLocation);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Fire_Event();

	bool GetCanFire();

	UFUNCTION(BlueprintCallable)
	void SetMuzzlePoint(USceneComponent* SceneComponent);
		
protected:

	bool CanFire;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	FString SoundTypeFire;

	FTimerHandle FireRateTimerHandle;

	UPROPERTY(EditAnywhere, Category = "VFX")
	TSoftObjectPtr<UNiagaraSystem> FireVFX;

	UPROPERTY()
	TEnumAsByte<ECollisionChannel> TraceChannelPorperty = ECC_Pawn;

	UPROPERTY(EditAnywhere, Category = "Stimuli")
	float FireSoundMaxRange = 2000;
	// Called when the game starts
	virtual void BeginPlay() override;

	FVector _FireTrace(FVector& direction, FVector startLocation);
	void _FinishFire();
};
