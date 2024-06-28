// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "SkillIndicator.generated.h"

UCLASS()
class BLOODRENEGADE_API ASkillIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkillIndicator();
	void Show(bool b);
	void ChangeFX(int index);
	bool GetEnoughSpace();
	void Move(FVector v);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	bool EnoughtPlace, IsActive;

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void BlindEffect(bool isShowing);

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void DecoyEffect(bool isShowing);

	UFUNCTION(Blueprintable, BlueprintImplementableEvent)
	void NotSpaceDecoyEffect(bool haveEnoughPlace);

	DECLARE_DELEGATE_OneParam(FEventFXSignature, bool);
	FEventFXSignature EventFX, EventNotSpaceFX;
	
	UFUNCTION()
	void CheckWall();
	
	UPROPERTY()
	FTimeline TimelineCheckWall;
};
