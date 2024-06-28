// Fill out your copyright notice in the Description page of Project Settings.


#include "AISenseConfig_CustomSight.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISenseConfig.h"
#include "VisualLogger/VisualLogger.h"

UAISenseConfig_CustomSight::UAISenseConfig_CustomSight(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DebugColor = FColor::Green;
}

TSubclassOf<UAISense> UAISenseConfig_CustomSight::GetSenseImplementation() const
{
	return *Implementation;
}

#if WITH_EDITOR
void UAISenseConfig_CustomSight::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	static const FName NAME_AutoSuccessRangeFromLastSeenLocation = GET_MEMBER_NAME_CHECKED(UAISenseConfig_CustomSight, AutoSuccessRangeFromLastSeenLocation);

	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		const FName PropName = PropertyChangedEvent.Property->GetFName();
		if (PropName == NAME_AutoSuccessRangeFromLastSeenLocation)
		{
			if (AutoSuccessRangeFromLastSeenLocation < 0)
			{
				AutoSuccessRangeFromLastSeenLocation = FAISystem::InvalidRange;
			}
		}
	}
}
#endif // WITH_EDITOR

#if WITH_GAMEPLAY_DEBUGGER_MENU
void UAISenseConfig_CustomSight::DescribeSelfToGameplayDebugger(const UAIPerceptionComponent* PerceptionComponent, FGameplayDebuggerCategory* DebuggerCategory) const
{
	/*if (PerceptionComponent == nullptr || DebuggerCategory == nullptr)
	{
		return;
	}

	FColor SightRangeColor = FColor::Green;
	FColor LoseSightRangeColor = FColorList::NeonPink;

	// don't call Super implementation on purpose, replace color description line
	DebuggerCategory->AddTextLine(
		FString::Printf(TEXT("%s: {%s}%s {white}rangeIN:{%s} %.2f (%s) {white} rangeOUT:{%s} %.2f (%s)"), *GetSenseName(),
			*GetDebugColor().ToString(), *DescribeColorHelper(GetDebugColor()),
			*SightRangeColor.ToString(), SightRadius, *DescribeColorHelper(SightRangeColor),
			*LoseSightRangeColor.ToString(), LoseSightRadius, *DescribeColorHelper(LoseSightRangeColor))
	);

	const AActor* BodyActor = PerceptionComponent->GetBodyActor();
	if (BodyActor != nullptr)
	{
		FVector BodyLocation, BodyFacing;
		PerceptionComponent->GetLocationAndDirection(BodyLocation, BodyFacing);

		DebuggerCategory->AddShape(FGameplayDebuggerShape::MakeCylinder(BodyLocation, LoseSightRadius, 25.0f, LoseSightRangeColor));
		DebuggerCategory->AddShape(FGameplayDebuggerShape::MakeCylinder(BodyLocation, SightRadius, 25.0f, SightRangeColor));

		const float SightPieLength = FMath::Max(LoseSightRadius, SightRadius) + PointOfViewBackwardOffset;
		const FVector RootLocation = BodyLocation - (BodyFacing * PointOfViewBackwardOffset);
		const FVector LeftDirection = BodyFacing.RotateAngleAxis(PeripheralVisionAngleDegrees, FVector::UpVector);
		const FVector RightDirection = BodyFacing.RotateAngleAxis(-PeripheralVisionAngleDegrees, FVector::UpVector);
		DebuggerCategory->AddShape(FGameplayDebuggerShape::MakeSegment(RootLocation + (BodyFacing * NearClippingRadius), RootLocation + (BodyFacing * SightPieLength), SightRangeColor));
		DebuggerCategory->AddShape(FGameplayDebuggerShape::MakeSegment(RootLocation + (LeftDirection * NearClippingRadius), RootLocation + (LeftDirection * SightPieLength), SightRangeColor));
		DebuggerCategory->AddShape(FGameplayDebuggerShape::MakeSegment(RootLocation + (RightDirection * NearClippingRadius), RootLocation + (RightDirection * SightPieLength), SightRangeColor));
		DebuggerCategory->AddShape(FGameplayDebuggerShape::MakeSegment(RootLocation + (LeftDirection * NearClippingRadius), RootLocation + (BodyFacing * NearClippingRadius), SightRangeColor));
		DebuggerCategory->AddShape(FGameplayDebuggerShape::MakeSegment(RootLocation + (BodyFacing * NearClippingRadius), RootLocation + (RightDirection * NearClippingRadius), SightRangeColor));
	}*/
}
#endif // WITH_GAMEPLAY_DEBUGGER_MENU