// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "CustomSpringArm.generated.h"

class ABloodPlayerState;
class ABloodController;
class ABloodCharacter;
class UBloodCharacterDataAsset;


UCLASS()
class BLOODRENEGADE_API UCustomSpringArm : public USpringArmComponent
{
	GENERATED_BODY()
public:	
	UPROPERTY(EditAnywhere)
	float AimCamOffset;
	
	FVector InitialCamOffset;

	void EnableAimCamPosition(float dir);
	void DisableAimCamPosition();
	void SetCameraArmDist(float f);
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UBloodCharacterDataAsset> DataAsset;

	UCustomSpringArm();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;
	
private:
	UPROPERTY()
	FTimeline MoveCamLocalTimeline;
	
	FVector NextLocalCamOffset, LastLocalCamPosition, NextWorldCamPosition, LastWorldCamPosition;
	float DistClampBySpeed;
	UCurveFloat* CurveLocalMove;
	TWeakObjectPtr<ABloodCharacter> ActorOwner;
	TWeakObjectPtr<ABloodPlayerState> CurrentPlayerState;
	
	UFUNCTION()
	void LerpLocalCamNextPos(float value);
};
