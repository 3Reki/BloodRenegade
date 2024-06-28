#pragma once

#include "CoreMinimal.h"
#include "LampUV.h"
#include "Components/TimelineComponent.h"
#include "SearchingLampUV.generated.h"

class USplineComponent;

UCLASS()
class BLOODRENEGADE_API ASearchingLampUV : public ALampUV
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere)
	TArray<float> PointsTiming;
	
private:

	UFUNCTION()
	void RotateTowardPoint(float Value);

	UFUNCTION()
	void Next();

	void StartTimelineCheck();
	
	ASearchingLampUV();
	virtual void BeingBlind_Implementation(float f) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	void LocalSwitchOff();
	void LocalSwitchOn();
	
	UPROPERTY()
	FTimeline TimelineForRevolution;

	FTimerHandle WaitForNextPointTimerHandle;
	UCurveFloat* RevolutionCurve;
	float NextPoint;
	bool Reverse;
};
