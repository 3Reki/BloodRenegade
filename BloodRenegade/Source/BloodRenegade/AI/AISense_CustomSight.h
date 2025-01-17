// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Perception/AISense_Sight.h"
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GenericTeamAgentInterface.h"
#include "WorldCollision.h"
#include "Misc/MTAccessDetector.h"
#include "Perception/AISense.h"
#include "AISense_CustomSight.generated.h"

class IAISightTargetInterface;
class UAISenseConfig_CustomSight;

namespace ECustomSightPerceptionEventName
{
	enum Type
	{
		Undefined,
		GainedSight,
		LostSight
	};
}

USTRUCT()
struct BLOODRENEGADE_API FAICustomSightEvent
{
	GENERATED_USTRUCT_BODY()

	typedef UAISense_CustomSight FSenseClass;

	float Age;
	ECustomSightPerceptionEventName::Type EventType;

	UPROPERTY()
	TObjectPtr<AActor> SeenActor;

	UPROPERTY()
	TObjectPtr<AActor> Observer;

	FAICustomSightEvent() : SeenActor(nullptr), Observer(nullptr) {}

	FAICustomSightEvent(AActor* InSeenActor, AActor* InObserver, ECustomSightPerceptionEventName::Type InEventType)
		: Age(0.f), EventType(InEventType), SeenActor(InSeenActor), Observer(InObserver)
	{
	}
};

struct BLOODRENEGADE_API FAICustomSightTarget
{
	typedef uint32 FTargetId;
	static const FTargetId InvalidTargetId;

	TWeakObjectPtr<AActor> Target;
	IAISightTargetInterface* SightTargetInterface;
	FGenericTeamId TeamId;
	FTargetId TargetId;

	FAICustomSightTarget(AActor* InTarget = NULL, FGenericTeamId InTeamId = FGenericTeamId::NoTeam);

	FORCEINLINE FVector GetLocationSimple() const
	{
		const AActor* TargetPtr = Target.Get();
		return TargetPtr ? TargetPtr->GetActorLocation() : FVector::ZeroVector;
	}

	FORCEINLINE const AActor* GetTargetActor() const { return Target.Get(); }
};

struct FAICustomSightQuery
{
	FPerceptionListenerID ObserverId;
	FAICustomSightTarget::FTargetId TargetId;

	float Score;
	float Importance;

	FVector LastSeenLocation;

	/** User data that can be used inside the IAISightTargetInterface::CanBeSeenFrom method to store a persistence state */
	mutable int32 UserData;

	union
	{
		/**
		 * We can share the memory for these values because they aren't used at the same time :
		 * - The FrameInfo is used when the query is queued for an update at a later frame. It stores the last time the
		 *   query was processed so that we can prioritize it accordingly against the other queries
		 * - The TraceInfo is used when the query has requested a asynchronous trace and is waiting for the result.
		 *   The engine guarantees that we'll get the info at the next frame, but since we can have multiple queries that
		 *   are pending at the same time, we need to store some information to identify them when receiving the result callback
		 */

		struct
		{
			uint64 bLastResult : 1;
			uint64 LastProcessedFrameNumber : 63;
		} FrameInfo;

		/**
		 * The 'FrameNumber' value can increase indefinitely while the 'Index' represents the number of queries that were
		 * already requested during this frame. So it shouldn't reach high values in the allocated 32 bits.
		 * Thanks to that we can reliable only use 31 bits for this value and thus have space to keep the bLastResult value
		 */
		struct
		{
			uint32 bLastResult : 1;
			uint32 Index : 31;
			uint32 FrameNumber;
		} TraceInfo;
	};

	FAICustomSightQuery(FPerceptionListenerID ListenerId = FPerceptionListenerID::InvalidID(), FAICustomSightTarget::FTargetId Target = FAICustomSightTarget::InvalidTargetId)
		: ObserverId(ListenerId), TargetId(Target), Score(0), Importance(0), LastSeenLocation(FAISystem::InvalidLocation), UserData(0)
	{
		FrameInfo.bLastResult = false;
		FrameInfo.LastProcessedFrameNumber = GFrameCounter;
	}

	/**
	 * Note: This should only be called on queries that are queued up for later processing (in SightQueriesOutOfRange or SightQueriesOutOfRange)
	 */
	float GetAge() const
	{
		return (float)(GFrameCounter - FrameInfo.LastProcessedFrameNumber);
	}

	/**
	 * Note: This should only be called on queries that are queued up for later processing (in SightQueriesOutOfRange or SightQueriesOutOfRange)
	 */
	void RecalcScore()
	{
		Score = GetAge() + Importance;
	}

	void OnProcessed()
	{
		FrameInfo.LastProcessedFrameNumber = GFrameCounter;
	}

	void ForgetPreviousResult()
	{
		LastSeenLocation = FAISystem::InvalidLocation;
		SetLastResult(false);
	}

	bool GetLastResult() const
	{
		return FrameInfo.bLastResult;
	}

	void SetLastResult(const bool bValue)
	{
		FrameInfo.bLastResult = bValue;
	}

	/**
	* Note: This only be called for pending queries because it will erase the LastProcessedFrameNumber value
	*/
	void SetTraceInfo(const FTraceHandle& TraceHandle)
	{
		check((TraceHandle._Data.Index & (static_cast<uint32>(1) << 31)) == 0);
		TraceInfo.Index = TraceHandle._Data.Index;
		TraceInfo.FrameNumber = TraceHandle._Data.FrameNumber;
	}

	class FSortPredicate
	{
	public:
		FSortPredicate()
		{}

		bool operator()(const FAICustomSightQuery& A, const FAICustomSightQuery& B) const
		{
			return A.Score > B.Score;
		}
	};
};

struct BLOODRENEGADE_API FAICustomSightQueryID
{
	FPerceptionListenerID ObserverId;
	FAICustomSightTarget::FTargetId TargetId;

	FAICustomSightQueryID(FPerceptionListenerID ListenerId = FPerceptionListenerID::InvalidID(), FAICustomSightTarget::FTargetId Target = FAICustomSightTarget::InvalidTargetId)
		: ObserverId(ListenerId), TargetId(Target)
	{
	}

	FAICustomSightQueryID(const FAICustomSightQuery& Query)
		: ObserverId(Query.ObserverId), TargetId(Query.TargetId)
	{
	}
};

//DECLARE_DELEGATE_FiveParams(FOnPendingVisibilityQueryProcessedDelegate, const FAICustomSightQueryID&, const bool, const float, const FVector&, const TOptional<int32>&);

UCLASS(ClassGroup=AI, config=Game)
class BLOODRENEGADE_API UAISense_CustomSight : public UAISense
{
	GENERATED_UCLASS_BODY()
	
public:
	struct FDigestedSightProperties
	{
		float PeripheralVisionAngleCos;
		
		float SightRadiusSq;
		float ShootRadiusSq;
		float CloseSightRadiusSq;
		float AutoSuccessRangeSqFromLastSeenLocation;
		float LoseSightRadiusSq;
		float PointOfViewBackwardOffset;
		float NearClippingRadiusSq;
		float TopClamp;
		float BottomClamp;
		uint8 AffiliationFlags;

		FDigestedSightProperties();
		FDigestedSightProperties(const UAISenseConfig_CustomSight& SenseConfig);
	};

	enum class EVisibilityResult
	{
		Visible,
		NotVisible,
		Pending
	};

	typedef TMap<FAICustomSightTarget::FTargetId, FAICustomSightTarget> FTargetsContainer;
	FTargetsContainer ObservedTargets;
	TMap<FPerceptionListenerID, FDigestedSightProperties> DigestedProperties;

	/** The SightQueries are a n^2 problem and to reduce the sort time, they are now split between in range and out of range */
	/** Since the out of range queries only age as the distance component of the score is always 0, there is few need to sort them */
	/** In the majority of the cases most of the queries are out of range, so the sort time is greatly reduced as we only sort the in range queries */
	int32 NextOutOfRangeIndex = 0;
	bool bSightQueriesOutOfRangeDirty = true;
	TArray<FAICustomSightQuery> SightQueriesOutOfRange;
	TArray<FAICustomSightQuery> SightQueriesInRange;
	TArray<FAICustomSightQuery> SightQueriesPending;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI Perception", config)
	int32 MaxTracesPerTick;

	/** Maximum number of asynchronous traces that can be requested in a single update call*/
	UPROPERTY(EditDefaultsOnly, Category = "AI Perception", config)
	int32 MaxAsyncTracesPerTick;

	UPROPERTY(EditDefaultsOnly, Category = "AI Perception", config)
	int32 MinQueriesPerTimeSliceCheck;

	UPROPERTY(EditDefaultsOnly, Category = "AI Perception", config)
	double MaxTimeSlicePerTick;

	UPROPERTY(EditDefaultsOnly, Category = "AI Perception", config)
	float HighImportanceQueryDistanceThreshold;

	float HighImportanceDistanceSquare;

	UPROPERTY(EditDefaultsOnly, Category = "AI Perception", config)
	float MaxQueryImportance;

	UPROPERTY(EditDefaultsOnly, Category = "AI Perception", config)
	float SightLimitQueryImportance;

	/** Defines the amount of async trace queries to prevent based on the number of pending queries at the start of an update.
	 * 1 means that the async trace budget is slashed by the pending queries count
	 * 0 means that the async trace budget is not impacted by the pending queries
	 */
	UPROPERTY(EditDefaultsOnly, Category = "AI Perception", config)
	float PendingQueriesBudgetReductionRatio;

	/** Defines if we are allowed to use asynchronous trace queries when there is no IAISightTargetInterface for a Target */
	UPROPERTY(EditDefaultsOnly, Category = "AI Perception", config)
	bool bUseAsynchronousTraceForDefaultSightQueries;

	ECollisionChannel DefaultSightCollisionChannel;

	//FOnPendingVisibilityQueryProcessedDelegate OnPendingCanBeSeenQueryProcessedDelegate;
	FTraceDelegate OnPendingTraceQueryProcessedDelegate;

	UE_MT_DECLARE_RW_ACCESS_DETECTOR(QueriesListAccessDetector);

public:
	//UAISense_CustomSight();

	virtual void PostInitProperties() override;

	void RegisterEvent(const FAICustomSightEvent& Event);

	virtual void RegisterSource(AActor& SourceActors) override;
	virtual void UnregisterSource(AActor& SourceActor) override;

	virtual void OnListenerForgetsActor(const FPerceptionListener& Listener, AActor& ActorToForget) override;
	virtual void OnListenerForgetsAll(const FPerceptionListener& Listener) override;

protected:
	virtual float Update() override;

	EVisibilityResult ComputeVisibility(UWorld* World, FAICustomSightQuery& SightQuery, FPerceptionListener& Listener, const AActor* ListenerActor, FAICustomSightTarget& Target, AActor* TargetActor, const FDigestedSightProperties& PropDigest, float& OutStimulusStrength, FVector& OutSeenLocation, int32& OutNumberOfLoSChecksPerformed, int32& OutNumberOfAsyncLosCheckRequested) const;
	virtual bool ShouldAutomaticallySeeTarget(const FDigestedSightProperties& PropDigest, FAICustomSightQuery* SightQuery, FPerceptionListener& Listener, AActor* TargetActor, float& OutStimulusStrength) const;
	void UpdateQueryVisibilityStatus(FAICustomSightQuery& SightQuery, FPerceptionListener& Listener, const bool bIsVisible, const FVector& SeenLocation, const float StimulusStrength, AActor* TargetActor, const FVector& TargetLocation) const;
	void OnPendingCanBeSeenQueryProcessed(const FAICustomSightQueryID& QueryID, const bool bIsVisible, const float StimulusStrength, const FVector& SeenLocation, const TOptional<int32>& UserData);
	void OnPendingTraceQueryProcessed(const FTraceHandle& TraceHandle, FTraceDatum& TraceDatum);
	void OnPendingQueryProcessed(const int32 SightQueryIndex, const bool bIsVisible, const float StimulusStrength, const FVector& SeenLocation, const TOptional<int32>& UserData, const TOptional<AActor*> InTargetActor = NullOpt);

	void OnNewListenerImpl(const FPerceptionListener& NewListener);
	void OnListenerUpdateImpl(const FPerceptionListener& UpdatedListener);
	void OnListenerRemovedImpl(const FPerceptionListener& RemovedListener);
	virtual void OnListenerConfigUpdated(const FPerceptionListener& UpdatedListener) override;

	void GenerateQueriesForListener(const FPerceptionListener& Listener, const FDigestedSightProperties& PropertyDigest, const TFunction<void(FAICustomSightQuery&)>& OnAddedFunc = nullptr);

	void RemoveAllQueriesByListener(const FPerceptionListener& Listener, const TFunction<void(const FAICustomSightQuery&)>& OnRemoveFunc = nullptr);
	void RemoveAllQueriesToTarget(const FAICustomSightTarget::FTargetId& TargetId, const TFunction<void(const FAICustomSightQuery&)>& OnRemoveFunc = nullptr);

	/** returns information whether new LoS queries have been added */
	bool RegisterTarget(AActor& TargetActor, const TFunction<void(FAICustomSightQuery&)>& OnAddedFunc = nullptr);

	float CalcQueryImportance(const FPerceptionListener& Listener, const FVector& TargetLocation, const float SightRadiusSq) const;
	bool RegisterNewQuery(const FPerceptionListener& Listener, const IGenericTeamAgentInterface* ListenersTeamAgent, const AActor& TargetActor, const FAICustomSightTarget::FTargetId& TargetId, const FVector& TargetLocation, const FDigestedSightProperties& PropDigest, const TFunction<void(FAICustomSightQuery&)>& OnAddedFunc);

	// Deprecated methods
public:
	UE_DEPRECATED(4.25, "Not needed anymore done automatically at the beginning of each update.")
		FORCEINLINE void SortQueries() {}

protected:
	enum FQueriesOperationPostProcess
	{
		DontSort,
		Sort
	};
	UE_DEPRECATED(4.25, "Use RemoveAllQueriesByListener without unneeded PostProcess parameter.")
		void RemoveAllQueriesByListener(const FPerceptionListener& Listener, FQueriesOperationPostProcess PostProcess) { RemoveAllQueriesByListener(Listener); }
	UE_DEPRECATED(4.25, "Use RemoveAllQueriesByListener without unneeded PostProcess parameter.")
		void RemoveAllQueriesByListener(const FPerceptionListener& Listener, FQueriesOperationPostProcess PostProcess, TFunctionRef<void(const FAICustomSightQuery&)> OnRemoveFunc) { RemoveAllQueriesByListener(Listener, [&](const FAICustomSightQuery& query) { OnRemoveFunc(query); }); }
	UE_DEPRECATED(4.25, "Use RemoveAllQueriesToTarget without unneeded PostProcess parameter.")
		void RemoveAllQueriesToTarget(const FAICustomSightTarget::FTargetId& TargetId, FQueriesOperationPostProcess PostProcess) { RemoveAllQueriesToTarget(TargetId); }
	UE_DEPRECATED(4.25, "Use RemoveAllQueriesToTarget without unneeded PostProcess parameter.")
		void RemoveAllQueriesToTarget(const FAICustomSightTarget::FTargetId& TargetId, FQueriesOperationPostProcess PostProcess, TFunctionRef<void(const FAICustomSightQuery&)> OnRemoveFunc) { RemoveAllQueriesToTarget(TargetId, [&](const FAICustomSightQuery& query) { OnRemoveFunc(query); }); }
	UE_DEPRECATED(4.25, "Use RegisterTarget without unneeded PostProcess parameter.")
		bool RegisterTarget(AActor& TargetActor, FQueriesOperationPostProcess PostProcess) { return RegisterTarget(TargetActor); }
	UE_DEPRECATED(4.25, "Use RegisterTarget without unneeded PostProcess parameter.")
		bool RegisterTarget(AActor& TargetActor, FQueriesOperationPostProcess PostProcess, TFunctionRef<void(FAICustomSightQuery&)> OnAddedFunc) { return RegisterTarget(TargetActor, [&](FAICustomSightQuery& query) { OnAddedFunc(query); }); }

};
