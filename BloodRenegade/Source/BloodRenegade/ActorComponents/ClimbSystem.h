// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "ClimbSystem.generated.h"

class ABloodPlayerState;
class ABloodCharacter;
class UBloodCharacterDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOODRENEGADE_API UClimbSystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClimbSystem();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool CheckVault();
	
	UPROPERTY()
	FTimeline TimelineCheckLedge;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UBloodCharacterDataAsset> DataAsset;
private:
	UFUNCTION()
	void CheckHighLedge();

	bool CheckLayer(float high);
	void ClimbWall(FVector impactPoint);
	void CharacterVisible();
	
	FHitResult RaycastResult;
	FCollisionQueryParams ParamRaycast;
	TWeakObjectPtr<ABloodCharacter> ActorOwner;
	TWeakObjectPtr<ABloodPlayerState> CurrentPlayerState;
	
	FTimerHandle TimerClimb;
};
