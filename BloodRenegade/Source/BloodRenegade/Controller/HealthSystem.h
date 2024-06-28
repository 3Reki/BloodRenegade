// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "HealthSystem.generated.h"

class ABloodCharacter;
class UBloodCharacterDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLOODRENEGADE_API UHealthSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void ResetHP();
	// Sets default values for this component's properties
	UHealthSystem();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void TakeHit(int Dmg);
	void WaitForResetHP(float f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UBloodCharacterDataAsset> DataAsset;
	
	UFUNCTION(BlueprintCallable)
	void SetHP(int Value);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* DeathMontage;

	UFUNCTION()
	void DeathReset();

	TWeakObjectPtr<ABloodCharacter> CurrentOwner;
	FTimerHandle TimingDeath, InvincibleTimerHandle, WaitResetTimerHandle;
};
