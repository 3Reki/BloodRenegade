// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VampiricVisionComponent.generated.h"

class AStaticMeshActor;
class ASoldier;
class ABloodController;
class ABloodHUD;
class ABloodPlayerState;
class ABloodCharacter;
class UBloodCharacterDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOODRENEGADE_API UVampiricVisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UVampiricVisionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void StartVision();
	void StopVision();
	void SwitchVision();
	
	AStaticMeshActor * SphereVisionVampiric;
	bool CanUseVV;
protected:

	virtual void BeginPlay() override;
	void DetectEnemy();

	
	
	UPROPERTY(EditAnywhere)
	UBloodCharacterDataAsset* DataAsset;

	FCollisionShape Shape;	
	bool IsVisionActif;
	FHitResult RaycastResult;
	TArray<FHitResult> RaycastResult2;
	FCollisionQueryParams ParamRaycast;
	TWeakObjectPtr<ABloodCharacter> ActorOwner;
	TWeakObjectPtr<ABloodPlayerState> CurrentPlayerState;
	TWeakObjectPtr<ABloodHUD> CurrentHUD;
	TWeakObjectPtr<APlayerCameraManager> PlayerCameraManager;
	TWeakObjectPtr<ABloodController> CurrentController;
	TArray<ASoldier*> Soldiers;
	TArray<ASoldier*> CurrentSoldiers;
	FTimerHandle VampiricTimerHandle;
};
