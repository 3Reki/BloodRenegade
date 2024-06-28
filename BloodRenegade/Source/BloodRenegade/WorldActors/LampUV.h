// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotherWorldElements.h"
#include "BloodRenegade/Interfaces/IBlindable.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "LampUV.generated.h"

class UW_BlindMarker;
class UWidgetComponent;
class ABloodCharacter;
class ABloodController;
class ULampUVDataAsset;
class UCapsuleComponent;

UCLASS()
class BLOODRENEGADE_API ALampUV : public AMotherWorldElements, public IIBlindable, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Blueprintable, BlueprintImplementableEvent, BlueprintCallable)
	void BlindEffect(bool b);
	
	ALampUV();
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
protected:
	virtual void BeingBlind_Implementation(float f) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	void InLight();
	void OutLight();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* LightSource;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variables|Data", meta = (AllowPrivateAccess = "true"))
	ULampUVDataAsset* DataAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* SoftLight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Flare1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Flare2;

	TWeakObjectPtr<ABloodCharacter> CurrentPlayer;
	TWeakObjectPtr<ABloodController> CurrentController;
	
	FTimerHandle DecreaseStaminaTimerHandle, SwitchOffTimer;

	FCollisionQueryParams ParamRaycast;
	FHitResult RaycastResult;
	bool InCollider, IsVisible;
	FTimerHandle TakeDamage;
	UCameraShakeBase* CameraShake;
	
	UPROPERTY()
	FGenericTeamId TeamID = 2;
};
