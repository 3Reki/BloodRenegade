// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AISenseConfig.h"
#include "AISense_CustomSight.h"
#include "AISenseConfig_CustomSight.generated.h"

/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UAISenseConfig_CustomSight : public UAISenseConfig
{
	GENERATED_UCLASS_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config, NoClear)
	TSubclassOf<UAISense_CustomSight> Implementation;

	/** Maximum sight distance to notice a target. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config, meta = (UIMin = 0.0, ClampMin = 0.0))
	float SightRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config)
	float ShootRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config)
	float CloseSightRadius;

	/** Maximum sight distance to see target that has been already seen. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config, meta = (UIMin = 0.0, ClampMin = 0.0))
	float LoseSightRadius;

	/** How far to the side AI can see, in degrees. Use SetPeripheralVisionAngle to change the value at runtime.
	 *	The value represents the angle measured in relation to the forward vector, not the whole range. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config, meta = (UIMin = 0.0, ClampMin = 0.0, UIMax = 180.0, ClampMax = 180.0, DisplayName = "PeripheralVisionHalfAngleDegrees"))
	float PeripheralVisionAngleDegrees;

	/** */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config)
	FAISenseAffiliationFilter DetectionByAffiliation;

	/** If not an InvalidRange (which is the default), we will always be able to see the target that has already been seen if they are within this range of their last seen location. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config)
	float AutoSuccessRangeFromLastSeenLocation;

	/** Point of view move back distance for cone calculation. In conjunction with near clipping distance, this will act as a close by awareness and peripheral vision. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config, meta = (UIMin = 0.0, ClampMin = 0.0))
	float PointOfViewBackwardOffset;

	/** Near clipping distance, to be used with point of view backward offset. Will act as a close by awareness and peripheral vision */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config, meta = (UIMin = 0.0, ClampMin = 0.0))
	float NearClippingRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config)
	float TopClamp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", config)
	float BottomClamp;

	virtual TSubclassOf<UAISense> GetSenseImplementation() const override;

#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

#if WITH_GAMEPLAY_DEBUGGER_MENU
	virtual void DescribeSelfToGameplayDebugger(const UAIPerceptionComponent* PerceptionComponent, FGameplayDebuggerCategory* DebuggerCategory) const;
#endif // WITH_GAMEPLAY_DEBUGGER_MENU
};
