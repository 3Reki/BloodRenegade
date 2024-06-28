// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPoint.generated.h"

class ASoldier;

UCLASS()
class BLOODRENEGADE_API APatrolPoint : public AActor
{
	GENERATED_BODY()

public:

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "default")
	TWeakObjectPtr<ASoldier> MasterSoldier;

	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	APatrolPoint();
	APatrolPoint(const FVector& Location, const FRotator& Rotation, float WaitTime);
	void BeginPlay() override;

	//GETTER SETTER
	FVector GetLocation() const;
	FRotator GetRotation() const;
	float GetWaitTime() const;

protected:
	UPROPERTY(EditAnywhere, category="default")
	FVector Location;

	UPROPERTY(EditAnywhere, category="default")
	FRotator Rotation;

	UPROPERTY(EditAnywhere, category="default")
	float WaitTime;
	
};
