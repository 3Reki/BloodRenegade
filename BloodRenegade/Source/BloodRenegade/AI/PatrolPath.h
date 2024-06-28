// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "PatrolPath.generated.h"

class UPatrolDataAsset;
class ASoldier;

USTRUCT(BlueprintType, Blueprintable)
struct FSplinePatrolPoint 
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float PPDistancePercentage = 0;
	UPROPERTY(EditAnywhere)
	FRotator PPRotator = FRotator(0);
	UPROPERTY(EditAnywhere)
	float WaitTime = 0;
};

UCLASS()
class BLOODRENEGADE_API APatrolPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolPath();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USplineComponent* SplineComponent;

	UFUNCTION(BlueprintCallable)
	void AddSoldier(ASoldier* Soldier);

	UFUNCTION(BlueprintCallable)
	void RemoveSoldier(ASoldier* Soldier);

	UFUNCTION(BlueprintCallable)
	bool ContainsSoldier(ASoldier* Soldier);

	UFUNCTION(BlueprintCallable)
	void SoldierReachedPoint(ASoldier* Soldier);

	UFUNCTION(BlueprintCallable)
	FVector GetPatrolPointLocation(ASoldier* Soldier);

	UFUNCTION(BlueprintCallable)
	FRotator GetPatrolPointRotation();

	UFUNCTION(BlueprintCallable)
	int GetNextPointIdx();

	UFUNCTION(BlueprintCallable)
	void SetCurrentPatrolPointIdx(int Idx);

	UFUNCTION(BlueprintCallable)
	FSplinePatrolPoint GetPatrolPoint();

	UFUNCTION(BlueprintCallable)
	int GetNBPatrolPoints();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPatrolDataAsset* DAPatrol;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSplinePatrolPoint> PatrolPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool UsePathRotation = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsPathLooping = false;

	int CurrentPatrolPointIdx;

	UPROPERTY()
	TArray<ASoldier *> Soldiers;
	UPROPERTY()
	TArray<bool> HasReachedPoint;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//TArray<FVector> SquadOffset = { FVector(0, 0, 0), FVector(0, 200, 0), FVector(0, -200, 0), FVector(-200, 0, 0), FVector(-200, 200, 0), FVector(-200, -200, 0) };

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float ConstructionArrowLength = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	float ConstructionArrowSize = 1;

	int PathDirection = 1;

	FTimerHandle timer;

	UFUNCTION(BlueprintCallable)
	void CheckAllSoldiersReachedPoint();

#if WITH_EDITORONLY_DATA
	virtual void OnConstruction(const FTransform& Transform) override;
#endif
private:
	

};
