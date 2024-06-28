// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPCharacter.h"
#include "Components/TimelineComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "BloodRenegade/Interfaces/IBlindable.h"
#include "Templates/SharedPointer.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "BehaviorStates.h"
#include "../DataAssets/SoldierDataAsset.h"
#include "Soldier.generated.h"

class USoldierSoundDataAsset;
class UW_BlindMarker;
class UAkComponent;
class UAIManagerSubsystem;
class APatrolPoint;
class AWeapon;
class UWeaponComponent;
class USoldierDialogueComponent;
class UAIPerceptionComponent;
class UAISenseConfig_CustomSight;
class UAISenseConfig_Hearing;
class UAISenseConfig_Prediction;
class AGOAPController;
class UCurveFloat;
struct FAIRequestID;
struct FPathFollowingResult;
class UArrowComponent;
class USplineComponent;
class APatrolPath;
class UTextRenderComponent;
class ABloodHUD;
class UWidgetComponent;
class UW_EnemyDetection;
class UVibationDataAsset;

UCLASS()
class BLOODRENEGADE_API ASoldier : public AGOAPCharacter, public IIBlindable, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:

	TWeakObjectPtr<AGOAPController> GoapController;
	TWeakObjectPtr<ABloodHUD> CurrentHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoldierDataAsset* SoldierDA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UVibationDataAsset* VibrationDA;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USoldierSoundDataAsset* SoundDataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAIPerceptionComponent* AIPerceptionComponentRename;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAISenseConfig_CustomSight* AISenseConfigSightRename;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAISenseConfig_Hearing* AISenseConfigHearing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeaponComponent* WeaponComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoldierDialogueComponent* DialogueComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAkComponent* AkComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* VisionPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* UIAnchorPoint;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* FakeVisionCone;

	UPROPERTY(BlueprintReadWrite)
	bool IsStunned = false;

	ASoldier();
	void Tick(float deltaTime) override;

	void InitializeActions() override;
	void InitializeImportantAtoms() override;
	void InitializeGoals() override;
	void InitializeActionsCosts() override;
	void CalculateActionsCosts() override;
	void CheckGoals() override;
	void ChangePlan() override;
	void InitializeEntity() override;
	void ChangeGoal(const FName NewGoalKey);
	void SetActiveFakeCone(bool b);
	void IsFinishing();
	void IsKillInNF();
	//Actions
	UFUNCTION()
	bool Scout();

	UFUNCTION()
	void WaitPatrolPoint(float WaitTime);

	UFUNCTION()
	void EndPatrol(); //called when patrol on current point is ended

	UFUNCTION()
	void CoroutineDetection(float value);

	UFUNCTION()
	void CoroutineHearingMultiplier(float value);

	void Suspicious();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SuspiciousEffect();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void NeutralEffect();

	void Alert();

	void SetDetectionSpeed();

	UFUNCTION()
	void CoroutineLoseDetection(float value);

	bool ChasePlayer();

	bool InEnemyRange();

	UFUNCTION(BlueprintCallable)
	bool Aim();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void AimEffect(FVector Direction);

	UFUNCTION(BlueprintCallable)
	void StopAim();

	UFUNCTION(BlueprintCallable)
	void ShootRifle();

	UFUNCTION(BlueprintCallable)
	void EndShootRifle();

	UFUNCTION()
	bool Strike();

	UFUNCTION(BlueprintCallable)
	void EndStrike();

	UFUNCTION(BlueprintCallable)
	bool Fire();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void FireEffect(FVector TargetPosition, FVector Direction);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ShieldEffect();

	bool SearchForEnemy();

	void FindNewSearchLocation();

	UFUNCTION()
	void EndWaiting();

	UFUNCTION()
	void EndSearchForEnemy();

	bool Move(FVector nextLocation);

	void Rotate();

	UFUNCTION()
	void CoroutineRotate(float value);

	UFUNCTION()
	void RotateToPlayer();

	void OnPatrolPointReached(FAIRequestID requestID, const FPathFollowingResult& result);

	void OnInteractableObjectReached(FAIRequestID requestID, const FPathFollowingResult& result);

	void OnLastSeenLocationReached(FAIRequestID requestID, const FPathFollowingResult& result);

	UFUNCTION(BlueprintCallable)
	void EndInteract();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Revive();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void BeingBlind(float f);

	UFUNCTION(BlueprintCallable)
	void BiteSound_Implementation(bool IsSilent, bool PlayLongSound);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void BiteDissolutionEvent();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Stun_Event(float t);

	UFUNCTION()
	void Unstun();

	UFUNCTION(BlueprintCallable)
	void Blind(float t);

	// EndBlind and AlertEnemies
	UFUNCTION(BlueprintCallable)
	void EndBlind();

	UFUNCTION(BlueprintCallable)
	void AlertEnemies();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Blind_Event(float t);

	UFUNCTION(BlueprintCallable)
	bool Interact();

	UFUNCTION(BlueprintCallable)
	void Use();

	UFUNCTION()
	bool GetHasFinishedMovement();

	UFUNCTION()
	bool GetHasEndedPatrol();

	UFUNCTION()
	bool GetHasEndedSearch();

	UFUNCTION()
	void SetCanStrikePlayer(bool b);

	UFUNCTION()
	bool GetHasEndedStrike();

	TArray<TWeakObjectPtr<AActor>> GetTargetStimuliActors();

	UFUNCTION(BlueprintCallable)
	void FindPriorityTarget();

	UFUNCTION(BlueprintCallable)
	bool TargetStimuliActorsContains(AActor* actor);

	float GetFireRange();

	////// START PROTO MIND CONTROL //////
	UFUNCTION()
	float GetDetectionPercentage() const;

	UFUNCTION()
	void SetDetectionPercentage(float f);
	////// END PROTO MIND CONTROL //////

	UFUNCTION(BlueprintCallable)
	void SetObjectToInteract(TSoftObjectPtr<AActor> object);

	UFUNCTION(BlueprintCallable)
	TSoftObjectPtr<AActor> GetObjectToInteract();

	UFUNCTION(BlueprintCallable)
	void SetIsBeingBitten(bool b);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetCustomStencil(int i);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void StartVampiricVisible(float f);

	UFUNCTION(BlueprintCallable)
	bool EnemyVisible();

	UFUNCTION(BlueprintCallable)
	bool GetIsDead();

	UFUNCTION(BlueprintCallable)
	bool GetSoundProcessSFE();

	UFUNCTION(BlueprintCallable)
	bool GetHasSeenEnemyRecently();

	UFUNCTION(BlueprintCallable)
	bool GetIsInEnemySight();

	UFUNCTION(BlueprintCallable)
	bool GetIsEnemyHeared();

	UFUNCTION(BlueprintCallable)
	bool GetIsShielded();

	UFUNCTION()
	float GetFitnessByRange(const FVector& Location) const;

	UFUNCTION(BlueprintCallable)
	BehaviorStates GetCurrentBehaviorState();

	UFUNCTION(BlueprintCallable)
	BehaviorStates GetLastBehaviorState();

	UFUNCTION(BlueprintCallable)
	void SetBehaviorStates(BehaviorStates NewState);

	void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const override;

	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION(BlueprintCallable)
	void ForgetHearingStimuli(AActor* actor, bool b);

	UFUNCTION()
	void DelegateOnTargetPerceptionUpdated(AActor* actor, FAIStimulus stimulus);

	void OnTargetSightPerceptionUpdated(FAIStimulus& stimulus, AActor* actor, FName& lastSeenLocationKey);

	void FindCorpse(ASoldier* Soldier, AActor*& actor, FName& lastSeenLocationKey);

	void EnterSight(AActor* actor);

	void ExitSight(AActor* actor, FName& lastSeenLocationKey, FAIStimulus& stimulus);

	void OnTargetHearingPerceptionUpdated(FAIStimulus& stimulus, AActor* actor, FName& lastSeenLocationKey);

	UFUNCTION(BlueprintCallable)
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, const class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetDetectionIndicatorVisible(bool Visible);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void LoadSave();

protected:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UAIManagerSubsystem> AIManagerSubsystem;
	
	UPROPERTY(BlueprintReadOnly)
	UW_EnemyDetection* EnemyDetectionWidget;

	UPROPERTY(EditAnywhere)
	FGenericTeamId TeamID = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool StartEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool IsDead;

	UPROPERTY(BlueprintReadWrite)
	bool IsMarked;

	UPROPERTY(BlueprintReadWrite)
	bool WasSuspicious;

	UPROPERTY(BlueprintReadWrite)
	bool CanBePossessed;

	UPROPERTY(BlueprintReadWrite)
	bool VibrationThresholdActivated = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default")
	//TArray<TSoftObjectPtr<APatrolPoint>> PatrolPath = TArray<TSoftObjectPtr<APatrolPoint>>();
	APatrolPath* PatrolPath;
	FTimerHandle PatrolPointTimerHandle;

	float CurrentPatrolDistance;
	bool HasFinishedMovement;
	bool HasEndedPatrol;

	//bool _hasEndedSearch;

	UPROPERTY(EditAnywhere, Category = "AI/Detection")
	UCurveFloat* CurveDetection;
	UPROPERTY()
	FTimeline TimelineDetection;
	UPROPERTY()
	FTimeline TimelineLoseDetection;

	UPROPERTY()
	FTimeline TimelineHearingDetection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	UMaterialParameterCollection* MPCSoldier;

	UPROPERTY(BlueprintReadOnly)
	bool IsEnemyInSight;

	UPROPERTY(BlueprintReadOnly)
	bool IsEnemyHeared;

	UPROPERTY(BlueprintReadWrite)
	float DetectionPercentage;
	 
	float DetectionSpeed = 50;
	float DetectionSpeedMultiplier = 1;
	float DetectionHearingMultiplier = 1;

	float CurrentLoseDetectionSpeed;
	bool CanLoseDetectionHearing = false;	
	bool CanLoseDetectionSearch = false;

	FQuat InitialRotation;
	FQuat DesiredRotation;

	FTimerHandle EndPatrolTimerHandle;
	FTimerHandle StunTimerHandle;
	FTimerHandle AimTimerHandle;
	FTimerHandle StrikeTimerHandle;
	FTimerHandle LoseStimuliTimerHandle;
	FTimerHandle LoseShieldTimerHandle;

	FTimerHandle EndBlindTimerHandle;
	FTimerHandle EndWaitOnSearchTimerHandle;
	FTimerHandle CanLoseDetectionTimerHandle;
	FTimerHandle DetectionStencilTimerHandle;
	FTimerDelegate CanLoseDetectionTimerDelegate;

	UPROPERTY()
	FTimeline TimelineRotation;

	FAISenseID IDSight;
	FAISenseID IDHearing;

	UPROPERTY(BlueprintReadOnly)
	bool IsShielded;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> TargetStimuliActors;
	int TargetIdx;
	FVector LastKnownLocation;

	bool canRemovePlayerStimuli = true;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> AlreadyFoundCorpses;

	TWeakObjectPtr<AActor> ObjectToInteract;

	bool HasReachedLastSeenPosition = false;
	bool HasFinishedWaiting = true;
	bool HasEndedSearch = true;
	bool HasEndedStrike = false;

	bool SoundProcessSFE = true;
	bool HasSeenDeadBody = false;

	bool OutlineActivated = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool VVActivated;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UCurveFloat* HearingMultiplierCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement/SearchEnemy")
	UCurveFloat* CurveSearch;

	UPROPERTY()
	FTimeline TimelineSearch;

	UPROPERTY()
	FTimeline TimelineWaitTimeOnSearchLocation;

	UPROPERTY(EditAnywhere, Category = "Collision")
	TEnumAsByte<ECollisionChannel> TraceChannelProperty = ECC_Pawn;

	UPROPERTY(BlueprintReadOnly)
	bool IsInEnemyRange;

	UPROPERTY(BlueprintReadOnly)
	bool IsInteracting = false;

	UPROPERTY(BlueprintReadOnly)
	bool IsAiming = false;

	UPROPERTY(BlueprintReadOnly)
	bool IsShooting = false;

	UPROPERTY(BlueprintReadOnly)
	bool StrikeUp = true;

	UPROPERTY(BlueprintReadOnly)
	bool IsStriking = false;

	UPROPERTY(BlueprintReadOnly)
	bool IsBeingBitten = false;

	BehaviorStates CurrentState;
	BehaviorStates LastState;

	UPROPERTY()
	TArray<ASoldier*> SoldiersInRange;

	FHitResult Hit;
	FCollisionQueryParams QueryParams;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEnvQuery* ChaseQuery;
	FEnvQueryRequest ChaseRequest;
	
	void ChaseQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void UnPossessed() override;

	UFUNCTION(BlueprintCallable)
	void Possessed();

	UFUNCTION(BlueprintCallable)
	void Revive_Implementation();

	UFUNCTION(BlueprintCallable)
	void Die_Implementation();

	UFUNCTION(BlueprintCallable)
	void BeingAttacked_Implementation();

	UFUNCTION(BlueprintCallable)
	void BeingArialBite_Implementation();
	
	UFUNCTION(BlueprintCallable)
	void Stun_Implementation(float t);

	UFUNCTION(BlueprintCallable)
	void ActivateShield();

	UFUNCTION(BlueprintCallable)
	void DeactivateShield();

	UFUNCTION(BlueprintCallable)
	void ChangeOutline(float value, FLinearColor color);

	void RemoveStimuli();

	
#if WITH_EDITORONLY_DATA
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	////// START PROTO MIND CONTROL //////

	TWeakObjectPtr<AGOAPController> GetGOAPController();

	TSoftObjectPtr<UAISenseConfig_CustomSight> GetAISenseConfigSight();
};
