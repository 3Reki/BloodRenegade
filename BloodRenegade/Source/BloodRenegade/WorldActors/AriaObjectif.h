// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "AriaObjectif.generated.h"

class UCharacterSoundDataAsset;
class UBoxComponent;
class UAriasDataAsset;
class AAriasMechanisme;
class ABloodCharacter;
class USplineComponent;
class UCapsuleComponent;

UENUM(BlueprintType)
enum class EAriaActions : uint8
{
	DeleteAria  UMETA(DisplayName="Delete Aria"),
	SpawnAria  UMETA(DisplayName="Spawn Aria"),
	ReAbleMovementPlayer  UMETA(DisplayName="Able Movement Player"),
	Wait UMETA(DisplayName="Wait"),
	PlayDialogue UMETA(DisplayName="PlayDialogue"),
	MoveToNextPoint UMETA(DisplayName="MoveToNextPoint"),
	SetActiveEscort UMETA(DisplayName="SetActiveEscort"),
	ResetPlayerCamera UMETA(DisplayName="ResetCameraPlayer"),
	UseGenerator UMETA(DisplayName="Use Generator"),
};

USTRUCT(BlueprintType)
struct FActions
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Blueprintable, BlueprintReadOnly)
	float SpeedToNextPoint;

	UPROPERTY(EditAnywhere, Blueprintable, BlueprintReadOnly)
	float WaitingTimer;

	UPROPERTY(EditAnywhere, Blueprintable, BlueprintReadOnly)
	EAriaActions Action;
};

UCLASS()
class BLOODRENEGADE_API AAriaObjectif : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variables|Data")
	TSoftObjectPtr<AActor> CurrentGenerator;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variables|Data")
	UCharacterSoundDataAsset* SoundDataAsset;
	
	UPROPERTY(EditAnywhere)
	FVector WalkingOffset;
	
	UPROPERTY(EditAnywhere)
	UAriasDataAsset* DataAsset;

	UPROPERTY(EditAnywhere, Blueprintable, BlueprintReadOnly)
	TArray<FActions> PointsActionTiming;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void ExecutAction(EAriaActions Action);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void InitAria(ABloodCharacter* Chara); 
	
	UFUNCTION(BlueprintCallable)
	void NextObjectif();

	UFUNCTION(BlueprintCallable)
	void SetCurrentObj(int f);

	UFUNCTION(BlueprintCallable)
	void StartObject();

	UFUNCTION(BlueprintCallable)
	float GetTimingWithPoint();

	UFUNCTION(BlueprintCallable)
	void SetActiveEscort(ABloodCharacter* Chara, bool b);

	UFUNCTION(BlueprintImplementableEvent)
	void StartEmotionTimeline();
protected:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USplineComponent* SplineComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditAnywhere, Category=Colision, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCapsuleComponent> CapsuleCollision;

	UPROPERTY()
	FTimeline TimelineMoveNextPosition;

	UPROPERTY(Blueprintable, EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UAnimInstance> CurrentAnims;

	ABloodCharacter* CurrentPlayer;
	UCurveFloat* MoveNextObjCurve;
	FTimerHandle TimerBetweenPoint;
	float CurrentPoint;
	bool IsDuoing, IsMoving;
	
	UFUNCTION()
	void MoveNextPoint(float Value);

	UFUNCTION()
	void FinishedMovedPoint();

	AAriaObjectif();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
