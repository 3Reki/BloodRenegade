// Fill out your copyright notice in the Description page of Project Settings.



#include "Soldier.h"
#include "WeaponComponent.h"
#include "SoldierDialogueComponent.h"
#include "GOAPController.h"
#include "../Controller/BloodCharacter.h"
#include "Curves/CurveFloat.h"
#include "../Subsystems/World/AIManagerSubsystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/ChildActorComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/TimelineComponent.h"
#include "AITypes.h"
#include "BloodRenegade/Interfaces/IInteractable.h"
#include "PatrolPath.h"
#include "Perception/AIPerceptionComponent.h"
#include "AISenseConfig_CustomSight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "../Controller/BloodHUD.h"
#include "AkComponent.h"
#include "../Widgets/W_EnemyDetection.h"
#include "Components/WidgetComponent.h"
#include "../DataAssets/VibationDataAsset.h"
#include "BloodRenegade/Controller/BloodPlayerState.h"
#include "BloodRenegade/DataAssets/Sound/SoldierSoundDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"

ASoldier::ASoldier() 
{

	//goapController = Cast<AGOAPController>(GetController());
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	DialogueComponent = CreateDefaultSubobject<USoldierDialogueComponent>(TEXT("DialogueComponent"));
	AIPerceptionComponentRename = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	AISenseConfigSightRename = CreateDefaultSubobject<UAISenseConfig_CustomSight>(TEXT("SenseConfig Sight"));
	//AISenseConfigSight->Implementation = CreateDefaultSubobject<UAISense_CustomSight>(TEXT("CustomSight"));
	AISenseConfigSightRename->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfigSightRename->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfigSightRename->DetectionByAffiliation.bDetectNeutrals = false;

	AISenseConfigHearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("SenseConfig Hearing"));
	AISenseConfigHearing->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfigHearing->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfigHearing->DetectionByAffiliation.bDetectNeutrals = false;

	AIPerceptionComponentRename->ConfigureSense(*AISenseConfigSightRename);
	AIPerceptionComponentRename->ConfigureSense(*AISenseConfigHearing);

	FakeVisionCone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FakeVisionCone"));
	FakeVisionCone->SetupAttachment(RootComponent);

	AkComponent = CreateDefaultSubobject<UAkComponent>(TEXT("AKComponent"));
	AkComponent->SetupAttachment(RootComponent);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Enemy Detection Widget"));
	WidgetComponent->SetupAttachment(RootComponent);

	VisionPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Vision Point"));
	VisionPoint->SetupAttachment(GetMesh());

	UIAnchorPoint = CreateDefaultSubobject<USceneComponent>(TEXT("UI Anchor Point"));
	UIAnchorPoint->SetupAttachment(RootComponent);
}

void ASoldier::BeginPlay() 
{
	Super::BeginPlay();
	
	IDSight = AISenseConfigSightRename->GetSenseID();
	
	IDHearing = AISenseConfigHearing->GetSenseID();

	if (PatrolPath) 
	{
		PatrolPath->AddSoldier(this);
	}

	CurrentHUD = GetWorld()->GetFirstPlayerController()->GetHUD<ABloodHUD>();
	
	EnemyDetectionWidget = Cast<UW_EnemyDetection>(WidgetComponent->GetWidget());
	
	AIManagerSubsystem = TWeakObjectPtr<UAIManagerSubsystem>(GetWorld()->GetSubsystem<UAIManagerSubsystem>());
	AIManagerSubsystem->AddSoldier(this);
	
	if (GetGOAPController().IsValid()) 
	{
		//Bind OnMoveCompleted

		check (SoldierDA->CurveRotation) 
		{
			FOnTimelineFloat ProgressRotationFunction;
			ProgressRotationFunction.BindUFunction(this, FName("CoroutineRotate"));
			TimelineRotation.AddInterpFloat(SoldierDA->CurveRotation, ProgressRotationFunction);
		}

		check (CurveSearch) 
		{
			FOnTimelineFloat ProgressSearchWaitFunction;
			FOnTimelineEventStatic SearchWaitFinishedCallback;
			SearchWaitFinishedCallback.BindUFunction(this, "EndWaiting");
			TimelineWaitTimeOnSearchLocation.AddInterpFloat(CurveSearch, ProgressSearchWaitFunction);
			TimelineWaitTimeOnSearchLocation.SetTimelineFinishedFunc(SearchWaitFinishedCallback);
			TimelineWaitTimeOnSearchLocation.SetPlayRate(1 / SoldierDA->WaitTimeOnSearchLocation);
		}

		check (CurveDetection) 
		{
			FOnTimelineFloat ProgressDetectionFunction;
			ProgressDetectionFunction.BindUFunction(this, FName("CoroutineDetection"));
			TimelineDetection.AddInterpFloat(CurveDetection, ProgressDetectionFunction);
			TimelineDetection.SetLooping(true);

			FOnTimelineFloat ProgressLoseDetectionFunction;
			ProgressLoseDetectionFunction.BindUFunction(this, FName("CoroutineLoseDetection"));
			TimelineLoseDetection.AddInterpFloat(CurveDetection, ProgressLoseDetectionFunction);
			TimelineLoseDetection.SetLooping(true);
		}

		check(HearingMultiplierCurve)
		{
			FOnTimelineFloat ProgressHearingMultiplier;
			ProgressHearingMultiplier.BindUFunction(this, FName("CoroutineHearingMultiplier"));
			TimelineHearingDetection.AddInterpFloat(HearingMultiplierCurve, ProgressHearingMultiplier);
			//TimelineHearingDetection.SetLooping(true);
		}

		//Bind OnTargetPerceptionUpdated
		AIPerceptionComponentRename->OnTargetPerceptionUpdated.AddDynamic(this, &ASoldier::DelegateOnTargetPerceptionUpdated);

		FName lastSeenLocationKey = FName(TEXT("LastSeenLocation"));
		GoapController->BlackboardComp->SetValueAsVector(lastSeenLocationKey, FVector(50, 50, 50));

		DetectionSpeed = SoldierDA->DetectionSpeed;
		CurrentLoseDetectionSpeed = SoldierDA->LoseDetectionATSSpeed;
		CanLoseDetectionHearing = true;
		CanLoseDetectionSearch = true;
	}
	ChangeOutline(0, FLinearColor(1, 1, 1, 0));

	ChaseRequest = FEnvQueryRequest(ChaseQuery, this);

	LoadSave();
	
	if (!StartEnabled) 
	{
		UnPossessed();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetActorHiddenInGame(true);
	}
}

void ASoldier::UnPossessed()
{
	if (GetGOAPController().IsValid())
	{
		GoapController->StopBehaviorTree();
		GoapController->StopMovement();
		GoapController->SetActualWorldStateAtomValue("EnemyVisible", false);
	}
	if (PatrolPath) 
	{
		PatrolPath->RemoveSoldier(this);
	}
	DetectionPercentage = 0;
	AIPerceptionComponentRename->SetSenseEnabled(UAISense_CustomSight::StaticClass(), false);
	AIPerceptionComponentRename->SetSenseEnabled(UAISense_Hearing::StaticClass(), false);
	TimelineDetection.Stop();
	TimelineLoseDetection.Stop();
	TimelineSearch.Stop();
	TimelineRotation.Stop();
	TimelineWaitTimeOnSearchLocation.Stop();
	GetWorldTimerManager().ClearAllTimersForObject(this);

	Super::UnPossessed();
}

void ASoldier::Possessed() 
{
	if (PatrolPath && !PatrolPath->ContainsSoldier(this)) 
	{
		PatrolPath->AddSoldier(this);
	}
	const TWeakObjectPtr<AGOAPController> CurrentGoapController = GetGOAPController();
	check(CurrentGoapController.IsValid());
	CurrentGoapController->Possess(this);
	CurrentGoapController.Get()->RestartBehaviorTree();
	AIPerceptionComponentRename->SetSenseEnabled(UAISense_CustomSight::StaticClass(), true);
	AIPerceptionComponentRename->SetSenseEnabled(UAISense_Hearing::StaticClass(), true);
}

#if WITH_EDITORONLY_DATA

void ASoldier::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}
#endif

TWeakObjectPtr<AGOAPController> ASoldier::GetGOAPController() 
{
	if (GoapController.IsValid()) return GoapController;

	GoapController = Cast<AGOAPController>(this->GetController());
	return GoapController;
}

TSoftObjectPtr<UAISenseConfig_CustomSight> ASoldier::GetAISenseConfigSight() 
{
	if (AISenseConfigSightRename) return AISenseConfigSightRename;

	AISenseConfigSightRename = Cast<UAISenseConfig_CustomSight>(AIPerceptionComponentRename->GetSenseConfig(UAISense::GetSenseID(UAISenseConfig_CustomSight::StaticClass())));
	return AISenseConfigSightRename;
}

void ASoldier::Tick(float deltaTime) 
{
	Super::Tick(deltaTime);

	TimelineRotation.TickTimeline(deltaTime);
	TimelineWaitTimeOnSearchLocation.TickTimeline(deltaTime);
	TimelineDetection.TickTimeline(deltaTime);
	TimelineLoseDetection.TickTimeline(deltaTime);
	TimelineHearingDetection.TickTimeline(deltaTime);

	if (!IsEnemyHeared) 
	{
		DetectionHearingMultiplier = 1;
		TimelineHearingDetection.Stop();
	}
	IsEnemyHeared = false;

	//if (EnemyVisible() && InEnemyRange()) EnemyDetectionWidget->SetIsShooting(true);
	//else EnemyDetectionWidget->SetIsShooting(false);
}

void ASoldier::InitializeActions()
{
	//AGOAPController* goapController = Cast<AGOAPController>(GetController());
	if (GetGOAPController().IsValid())
	{
		ActionNames.Add(FName(TEXT("ChasePlayer")), *("ChasePlayer"));
		ActionNames.Add(FName(TEXT("ShootRifle")), *("ShootRifle"));
		ActionNames.Add(FName(TEXT("Strike")), *("Strike"));
		ActionNames.Add(FName(TEXT("SearchForEnemy")), *("SearchForEnemy"));
		ActionNames.Add(FName(TEXT("Scout")), *("Scout"));
		ActionNames.Add(FName(TEXT("Interact")), *("Interact"));

		//Action Scout
		GoapController->SetActionPre("Scout", "HasSeenEnemyRecently", false);
		GoapController->SetActionPre("Scout", "EnemyAlive", true);
		GoapController->SetActionPst("Scout", "HasSeenEnemyRecently", true);

		//Action ChasePlayer
		GoapController->SetActionPre("ChasePlayer", "EnemyVisible", true);
		GoapController->SetActionPst("ChasePlayer", "EnemyInRange", true);
		GoapController->SetActionPst("ChasePlayer", "EnemyInStrikeRange", true);

		//Action FireRifle
		GoapController->SetActionPre("ShootRifle", "EnemyVisible", true);
		GoapController->SetActionPre("ShootRifle", "CanStrike", false);
		GoapController->SetActionPre("ShootRifle", "EnemyInRange", true);
		GoapController->SetActionPst("ShootRifle", "EnemyAlive", false);

		//Action Strike
		GoapController->SetActionPre("Strike", "EnemyVisible", true);
		GoapController->SetActionPre("Strike", "CanStrike", true);
		GoapController->SetActionPre("Strike", "EnemyInRange", true);
		GoapController->SetActionPst("Strike", "EnemyAlive", false);

		//Action SearchForEnemy
		GoapController->SetActionPre("SearchForEnemy", "EnemyAlive", true);
		GoapController->SetActionPre("SearchForEnemy", "HasSeenEnemyRecently", true);
		GoapController->SetActionPre("SearchForEnemy", "EnemyVisible", false);
		GoapController->SetActionPst("SearchForEnemy", "EnemyVisible", true);
		GoapController->SetActionPst("SearchForEnemy", "WasAlerted", true);

		//Action Interact
		GoapController->SetActionPre("Interact", "InteractableState", false);
		GoapController->SetActionPst("Interact", "HasSeenEnemyRecently", true);
		GoapController->SetActionPst("Interact", "InteractableState", true);
	}
}

void ASoldier::InitializeImportantAtoms()
{
	ImportantAtoms.Add(FName(TEXT("LowHealth")), *("LowHealth"));
	ImportantAtoms.Add(FName(TEXT("EnemyAlive")), *("EnemyAlive"));
	ImportantAtoms.Add(FName(TEXT("EnemyVisible")), *("EnemyVisible"));
	ImportantAtoms.Add(FName(TEXT("EnemyInRange")), *("EnemyInRange"));
	ImportantAtoms.Add(FName(TEXT("HasSeenEnemyRecently")), *("HasSeenEnemyRecently"));
	ImportantAtoms.Add(FName(TEXT("WasAlerted")), *("WasAlerted"));
	ImportantAtoms.Add(FName(TEXT("InteractableState")), *("InteractableState"));
	ImportantAtoms.Add(FName(TEXT("CanStrike")), *("CanStrike"));
	ImportantAtoms.Add(FName(TEXT("EnemyInStrikeRange")), *("EnemyInStrikeRange"));
}

void ASoldier::InitializeGoals() 
{
	if (GetGOAPController().IsValid())
	{
		//Description of the actual worldstate
		GoapController->SetActualWorldStateAtomValue("LowHealth", false);
		GoapController->SetActualWorldStateAtomValue("EnemyAlive", true);
		GoapController->SetActualWorldStateAtomValue("EnemyVisible", false);
		GoapController->SetActualWorldStateAtomValue("EnemyInRange", false);
		GoapController->SetActualWorldStateAtomValue("HasSeenEnemyRecently", false);
		GoapController->SetActualWorldStateAtomValue("WasAlerted", false);
		GoapController->SetActualWorldStateAtomValue("InteractableState", true);
		GoapController->SetActualWorldStateAtomValue("CanStrike", false);
		GoapController->SetActualWorldStateAtomValue("EnemyInStrikeRange", false);

		//Description of the MAIN Goal
		TArray<TTuple<FName, bool>> MainGoal = TArray<TTuple<FName, bool>>();
		MainGoal.Emplace("EnemyAlive", false);
		Goals.Add("MAIN", MainGoal);

		last_goal_map_key = actual_goal_map_key = "MAIN";

		//Description of the LIGHT_DOWN Goal
		TArray<TTuple<FName, bool>> InteractableDownGoal = TArray<TTuple<FName, bool>>();
		InteractableDownGoal.Emplace("InteractableState", true);
		InteractableDownGoal.Emplace("EnemyAlive", false);
		Goals.Add("INTERACTABLE_DOWN", InteractableDownGoal);
	}
}

void ASoldier::InitializeActionsCosts()
{
	if (GetGOAPController().IsValid())
	{
		FName scoutName = FName(TEXT("Scout"));
		GoapController->SetActionCost(&scoutName, 2);

		FName Strike = FName("Strike");
		GoapController->SetActionCost(&Strike, 50);
	}
}

void ASoldier::CalculateActionsCosts()
{

}

void ASoldier::CheckGoals() 
{
	if (!GetGOAPController().IsValid()) return;
	TArray<TTuple<FName, bool>>* goal = Goals.Find(actual_goal_map_key);

	for (TTuple<FName, bool> atom : *goal) 
	{
		GoapController->SetDesiredWorldStateAtomValue(TCHAR_TO_ANSI(*atom.Key.ToString()), atom.Value);
	}
}

void ASoldier::ChangePlan() 
{
	if (GetGOAPController().IsValid()) 
	{
		GoapController->BlackboardComp->SetValueAsBool("PlanStateChanged", true);
		GoapController->BehaviorComp->RestartTree();
	}
}

void ASoldier::InitializeEntity() 
{

}

void ASoldier::ChangeGoal(const FName NewGoalKey) 
{
	if (NewGoalKey.IsEqual(actual_goal_map_key)) 
	{
		FName MainGoal = FName(TEXT("MAIN"));
	}
	else 
	{
		actual_goal_map_key = NewGoalKey;
		ChangePlan();
	}
}

void ASoldier::SetActiveFakeCone(bool b)
{
	if(FakeVisionCone)
	{
		FakeVisionCone->SetVisibility(b);
	}
}

void ASoldier::IsFinishing()
{
	GetMesh()->GetAnimInstance()->ProcessEvent(GetMesh()->GetAnimInstance()->FindFunction(FName("Finisher")), NULL);
}

void ASoldier::IsKillInNF()
{
	GetMesh()->GetAnimInstance()->ProcessEvent(GetMesh()->GetAnimInstance()->FindFunction(FName("GetNMK")), NULL);
}

bool ASoldier::Scout() 
{
	UPathFollowingComponent* pathFollowingComponent = GoapController->GetPathFollowingComponent();
	pathFollowingComponent->OnRequestFinished.RemoveAll(this);
	pathFollowingComponent->OnRequestFinished.AddUObject(this, &ASoldier::OnPatrolPointReached);

	GetCharacterMovement()->MaxWalkSpeed = SoldierDA->MaxScoutingWalkSpeed;

	if (!PatrolPath) return false;
	if (PatrolPath->GetNBPatrolPoints() < 1) return false;
	
	HasEndedPatrol = false;
	
	return Move(PatrolPath->GetPatrolPointLocation(this));
}

void ASoldier::WaitPatrolPoint(float WaitTime)
{
	WaitTime > 0 ? GetWorldTimerManager().SetTimer(EndPatrolTimerHandle, this, &ASoldier::EndPatrol, WaitTime, false) : EndPatrol();
}

void ASoldier::EndPatrol() 
{
	
	HasEndedPatrol = true;
}

void ASoldier::CoroutineDetection(float value) 
{
	if (!GetGOAPController().IsValid()) return;
	if (TargetStimuliActors.IsEmpty()) return;

	SetDetectionSpeed();
	if (DetectionPercentage <= 0 && DetectionSpeed * DetectionSpeedMultiplier > 0) 
	{
		ChangeOutline(0, FLinearColor(1, 1, 1, 0));
		CurrentHUD->AddEnemyMarker(this);
		GetWorldTimerManager().ClearTimer(LoseShieldTimerHandle);
		GetWorldTimerManager().SetTimer(LoseShieldTimerHandle, this, &ASoldier::DeactivateShield, SoldierDA->ShieldDuration);
	}
	DetectionPercentage += DetectionSpeed * GetWorld()->GetDeltaSeconds() * DetectionSpeedMultiplier;
	EnemyDetectionWidget->SetDetectionPercentage(DetectionPercentage);

	if (!VibrationThresholdActivated && DetectionPercentage >= SoldierDA->DetectionVibrationThreshold) 
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientPlayForceFeedback(VibrationDA->VisionDetection);
		VibrationThresholdActivated = true;
	}
	
	if (DetectionPercentage >= 100)
	{
		Suspicious();
	}
	if (DetectionPercentage >= 200) 
	{
		Alert();
	}
}

void ASoldier::CoroutineHearingMultiplier(float value)
{
	DetectionHearingMultiplier = value;
}

void ASoldier::Suspicious()
{
	
	if (!GoapController->BlackboardComp->GetValueAsBool("HasSeenEnemyRecently"))
	{
		WasSuspicious = true;
		ChangeOutline(2, FLinearColor(1, 0.92, 0, 0));
		DialogueComponent->PlayDialogue(BehaviorStates::Neutral, BehaviorStates::Suspect);
		GoapController->SetActualWorldStateAtomValue("HasSeenEnemyRecently", true);
		EnemyDetectionWidget->SetHasSeenEnemyRecently(true);
		AIManagerSubsystem.Get()->SetSoldierIsSuspicious(this, true);
		SuspiciousEffect();
	}

	FName lastSeenLocationKey = FName(TEXT("LastSeenLocation"));
	if (!TargetStimuliActors.IsEmpty())
	{
		GoapController->BlackboardComp->SetValueAsVector(lastSeenLocationKey, TargetStimuliActors[TargetStimuliActors.Num() - 1]->GetActorLocation());
	}
	else 
	{
		UNavigationSystemV1* navArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
		FNavLocation randomLocationInRadius;
		if (navArea && GetGOAPController().IsValid())
		{
			navArea->GetRandomReachablePointInRadius(GetActorLocation(), SoldierDA->SearchRadius, randomLocationInRadius);
		}
		GoapController->BlackboardComp->SetValueAsVector(lastSeenLocationKey, randomLocationInRadius.Location);
	}
	if (TimelineSearch.IsPlaying()) TimelineSearch.Stop();
}

void ASoldier::Alert()
{
	ChangeOutline(2, FLinearColor(1, 0, 0.25, 0));
	DetectionPercentage = 200;
	AIManagerSubsystem.Get()->SetSoldierIsAlerted(this, true);
	if (GoapController.Get()->BlackboardComp->GetValueAsBool("WasAlerted"))
	{
		DialogueComponent->PlayDialogue(BehaviorStates::Search, BehaviorStates::Alert);
	}
	else
	{
		DialogueComponent->PlayDialogue(BehaviorStates::Suspect, BehaviorStates::Alert);
		//UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientPlayForceFeedback(VibrationDA->SoldierAlerted);
	}
	GoapController->SetActualWorldStateAtomValue("EnemyVisible", true);
	GoapController->SetActualWorldStateAtomValue("WasAlerted", true);
	GetWorldTimerManager().ClearTimer(LoseShieldTimerHandle);
	
	if (!IsShielded)
		ActivateShield();
	
	TimelineDetection.Stop();
}

void ASoldier::SetDetectionSpeed()
{
	if (TargetStimuliActors.IsEmpty()) return;

	AActor* Target = nullptr;
	for (int i = 0; i < TargetStimuliActors.Num(); i++)
	{
		if (Cast<ABloodCharacter>(TargetStimuliActors[i]))
		{
			Target = TargetStimuliActors[i].Get();
		}
	}
	if (!Target) return;
	ABloodCharacter* BloodCharacter = Cast<ABloodCharacter>(Target);
	check(BloodCharacter);

	FVector toEnemy = BloodCharacter->GetActorLocation() - GetActorLocation();
	FVector toEnemyNormalized = toEnemy;
	toEnemyNormalized.Normalize();
	DetectionSpeed = SoldierDA->CurveCentralVisionDetectionSpeed->GetFloatValue((toEnemy.SquaredLength() / (3000 * 3000)));
	DetectionSpeed *= SoldierDA->CurvePeripheralVisionMultiplier->GetFloatValue(GetActorForwardVector().Dot(toEnemyNormalized));
	if (DetectionPercentage < 100) DetectionSpeed *= SoldierDA->CurveHeightVisionMultiplier->GetFloatValue(toEnemy.Z);
	if (BloodCharacter->CurrentPlayerState->IsCrouched) DetectionSpeed *= SoldierDA->PlayerCrouchedMultiplier;
}

void ASoldier::CoroutineLoseDetection(float value) 
{
	if (!GetGOAPController().IsValid() || !CanLoseDetectionHearing /* || !CanLoseDetectionSearch*/) return;

	if (DetectionPercentage >= 200)
	{
		ChangeOutline(2, FLinearColor(1, 0.92, 0, 0));
		AIManagerSubsystem.Get()->SetSoldierIsAlerted(this, false);
		GetWorldTimerManager().SetTimer(LoseShieldTimerHandle, this, &ASoldier::DeactivateShield, SoldierDA->ShieldDuration);
	}
	if (!GoapController.Get()->BlackboardComp->GetValueAsBool("HasSeenEnemyRecently")) 
	{
		CurrentLoseDetectionSpeed = SoldierDA->LoseDetectionNSSpeed;
	}
	else 
	{
		CurrentLoseDetectionSpeed = DetectionPercentage < 100 ? SoldierDA->LoseDetectionSTNSpeed : SoldierDA->LoseDetectionATSSpeed;
	}
	DetectionPercentage -= CurrentLoseDetectionSpeed * GetWorld()->GetDeltaSeconds();
	EnemyDetectionWidget->SetDetectionPercentage(DetectionPercentage);

	if (DetectionPercentage <= 100 && WasSuspicious) 
	{
		WasSuspicious = false;
		AIManagerSubsystem.Get()->SetSoldierIsSuspicious(this, false);
		NeutralEffect();
	}

	if (DetectionPercentage <= 0)
	{
		ChangeOutline(0, FLinearColor(1, 1, 1, 0));
		CurrentHUD->RemoveEnemyMarker(this);
		DetectionPercentage = 0;
		if (!GoapController.Get()->BlackboardComp->GetValueAsBool("WasAlerted") && !IsDead && !IsBeingBitten)
		{
			DialogueComponent->PlayDialogue(BehaviorStates::Search, BehaviorStates::Neutral);
		}
		GoapController->SetActualWorldStateAtomValue("WasAlerted", false);
		VibrationThresholdActivated = false;
		TimelineLoseDetection.Stop();
		EndSearchForEnemy();
	}
}

bool ASoldier::ChasePlayer() 
{
	UPathFollowingComponent* pathFollowingComponent = GoapController->GetPathFollowingComponent();
	pathFollowingComponent->OnRequestFinished.RemoveAll(this);

	if (!GetGOAPController().IsValid()) return false;
	if (TargetStimuliActors.IsEmpty()) return false;

	GetCharacterMovement()->MaxWalkSpeed = SoldierDA->MaxChasingWalkSpeed;

	HasFinishedMovement = false;
	CanLoseDetectionSearch = true;

	FName lastSeenLocation = FName(TEXT("LastSeenLocation"));
	AActor* Target = nullptr;
	for (int i = 0; i < TargetStimuliActors.Num(); i++)
	{
		if (Cast<ABloodCharacter>(TargetStimuliActors[i]))
		{
			Target = TargetStimuliActors[i].Get();
		}
	}
	if (!Target) return false;
	ChaseRequest.Execute(EEnvQueryRunMode::SingleResult, this, &ASoldier::ChaseQueryFinished);
	return true;
}

void ASoldier::ChaseQueryFinished(TSharedPtr<FEnvQueryResult> Result) 
{
	if (Result->IsSuccessful()) 
	{
		GoapController->MoveToLocation(Result->GetItemAsLocation(0), 5);
	}
}

bool ASoldier::InEnemyRange() 
{
	if (!GetGOAPController().IsValid()) return false;
	if (TargetStimuliActors.IsEmpty()) return false;
	AActor* Target = nullptr;
	for (int i = 0; i < TargetStimuliActors.Num(); i++)
	{
		if (Cast<ABloodCharacter>(TargetStimuliActors[i]))
		{
			Target = TargetStimuliActors[i].Get();
		}
	}
	if (Cast<ABloodCharacter>(Target)) 
	{
		if ((Target->GetActorLocation() - VisionPoint->GetComponentLocation()).Length() < SoldierDA->StrikeRange && StrikeUp)
		{
			IsInEnemyRange = true;
			GoapController->SetActualWorldStateAtomValue("EnemyInRange", true);
			GoapController->SetActualWorldStateAtomValue("CanStrike", true);
			return true;
		}
		else if ((Target->GetActorLocation() - VisionPoint->GetComponentLocation()).Length() < GetFireRange())
		{
			IsInEnemyRange = true;
			GoapController->SetActualWorldStateAtomValue("EnemyInRange", true);
			return true;
		}
		else
		{
			IsInEnemyRange = false;
			GoapController->SetActualWorldStateAtomValue("EnemyInRange", false);
			return false;
		}
	}
	return false;
}

bool ASoldier::Aim()
{
	UPathFollowingComponent* pathFollowingComponent = GoapController->GetPathFollowingComponent();
	pathFollowingComponent->OnRequestFinished.RemoveAll(this);

	GetCharacterMovement()->MaxWalkSpeed = SoldierDA->MaxShootingWalkSpeed;

	ABloodCharacter* Target = nullptr;
	for (int i = 0; i < TargetStimuliActors.Num(); i++)
	{
		if ((Target = Cast<ABloodCharacter>(TargetStimuliActors[i])) != nullptr) break;
	}
	
	if (!Target) return false;
	
	IsAiming = true;
	AimEffect(Target->VisiblePoint->GetComponentLocation() - WeaponComponent->MuzzlePoint->GetComponentLocation());
	GetWorldTimerManager().SetTimer(AimTimerHandle, this, &ASoldier::ShootRifle, SoldierDA->AimTime);
	
	
	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->ChargeShootEvent, this, 0, NullCallback);
	
	return true;
}

void ASoldier::StopAim()
{
	GetWorldTimerManager().ClearTimer(AimTimerHandle);

	IsAiming = false;
	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->ChargeShootStopEvent, this, 0, NullCallback);
}

void ASoldier::ShootRifle()
{
	IsShooting = true;
}

void ASoldier::EndShootRifle()
{
	IsShooting = false;
	GetWorldTimerManager().SetTimer(AimTimerHandle, this, &ASoldier::ShootRifle, SoldierDA->AimTime);
}

bool ASoldier::Strike()
{
	if (TargetStimuliActors.IsEmpty()) return false;
	AActor* Target = nullptr;
	for (int i = 0; i < TargetStimuliActors.Num(); i++)
	{
		if (Cast<ABloodCharacter>(TargetStimuliActors[i]))
		{
			Target = TargetStimuliActors[i].Get();
		}
	}
	if (!Target) return true;
	ABloodCharacter* BloodCharacter = Cast<ABloodCharacter>(Target);
	check(BloodCharacter);
	FVector actorLocation = FVector(GetActorLocation().X, GetActorLocation().Y, 0);
	FVector targetLocation = FVector(Target->GetActorLocation().X, Target->GetActorLocation().Y, 0);
	FRotator rotationVector = UKismetMathLibrary::FindLookAtRotation(actorLocation, targetLocation);
	BloodCharacter->Stunned(SoldierDA->StrikeStunDuration);
	IsStriking = true;
	HasEndedStrike = false;
	AIManagerSubsystem.Get()->SetCanStrikePlayer(false);
	GoapController->SetActualWorldStateAtomValue("CanStrike", false);
	return true;
}

void ASoldier::EndStrike()
{
	IsStriking = false;
	HasEndedStrike = true;
}

bool ASoldier::Fire() 
{
	if (TargetStimuliActors.IsEmpty()) return false;

	
	FVector actorLocation = FVector(GetActorLocation().X, GetActorLocation().Y, 0);
	ABloodCharacter* Target = nullptr;
	for (int i = 0; i < TargetStimuliActors.Num(); i++) 
	{
		if ((Target = Cast<ABloodCharacter>(TargetStimuliActors[i])) != nullptr) break;
	}
	
	if (!Target) return false;
	FireEffect(Target->VisiblePoint->GetComponentLocation(), Target->VisiblePoint->GetComponentLocation() - WeaponComponent->MuzzlePoint->GetComponentLocation());
	WeaponComponent->Fire(Target->VisiblePoint->GetComponentLocation() - VisionPoint->GetComponentLocation(), VisionPoint->GetComponentLocation());
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), this->GetActorLocation(), 1, this, SoldierDA->ShotSoundRange, FName(TEXT("Instant")));
	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->ShootEvent, this, 0, NullCallback);
	
	return true;
}

bool ASoldier::SearchForEnemy() 
{
	UPathFollowingComponent* pathFollowingComponent = GoapController->GetPathFollowingComponent();
	pathFollowingComponent->OnRequestFinished.RemoveAll(this);

	if (!GetGOAPController().IsValid()) return false;
	//UE_LOG(LogTemp, Warning, TEXT("Search For Enemy In Soldier.cpp"))
	SoundProcessSFE = true;
	if (HasSeenDeadBody) 
	{
		GetCharacterMovement()->MaxWalkSpeed = SoldierDA->MaxChasingWalkSpeed;
	}
	else 
	{
		GetCharacterMovement()->MaxWalkSpeed = SoldierDA->MaxSearchingWalkSpeed;
	} 
	GoapController->MoveToLocation(GoapController->BlackboardComp->GetValueAsVector("LastSeenLocation"), 20);
	if (!pathFollowingComponent->HasValidPath())
	{
		if (!IsEnemyInSight) 
		{
			CanLoseDetectionHearing = true;
			CanLoseDetectionSearch = true;
			TimelineLoseDetection.PlayFromStart();
			FindNewSearchLocation();
		}
		return true;
	}
	pathFollowingComponent->OnRequestFinished.AddUObject(this, &ASoldier::OnLastSeenLocationReached);
	HasFinishedMovement = false;
	HasEndedSearch = false;
	HasReachedLastSeenPosition = false;
	CanLoseDetectionSearch = false;

	return true;
}

void ASoldier::FindNewSearchLocation()
{
	UNavigationSystemV1* navArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
	if (navArea && GetGOAPController().IsValid() && HasFinishedWaiting)
	{
		HasFinishedMovement = false;
		UPathFollowingComponent* pathFollowingComponent = GoapController->GetPathFollowingComponent();
		pathFollowingComponent->OnRequestFinished.RemoveAll(this);
		pathFollowingComponent->OnRequestFinished.AddUObject(this, &ASoldier::OnLastSeenLocationReached);
		FNavLocation randomLocationInRadius;
		navArea->GetRandomReachablePointInRadius(GoapController.Get()->BlackboardComp->GetValueAsVector("LastSeenLocation"), SoldierDA->SearchRadius, randomLocationInRadius);
		GoapController->MoveToLocation(randomLocationInRadius.Location, 20);
		GetCharacterMovement()->MaxWalkSpeed = SoldierDA->MaxSearchingWalkSpeed;
	}
}

void ASoldier::EndWaiting() 
{
	HasFinishedWaiting = true;
	if (!HasEndedSearch) 
	{
		FindNewSearchLocation();
	}
}

void ASoldier::EndSearchForEnemy() 
{
	HasEndedSearch = true;
}

bool ASoldier::Move(FVector nextLocation) 
{
	//AGOAPController* goapController = Cast<AGOAPController>(GetController());
	if (!GetGOAPController().IsValid()) return false;
	HasFinishedMovement = false;
	GoapController->MoveToLocation(nextLocation);
	return true;
}

void ASoldier::Rotate() 
{
	if (!GetGOAPController().IsValid()) return;

	InitialRotation = GetActorQuat();
	DesiredRotation = PatrolPath->GetPatrolPointRotation().Quaternion();
	TimelineRotation.PlayFromStart();
}


void ASoldier::CoroutineRotate(float value) 
{
	//AGOAPController* goapController = Cast<AGOAPController>(GetController());
	if (!GetGOAPController().IsValid()) return;

	GoapController->GetPawn()->SetActorRotation(FQuat::Slerp(InitialRotation, DesiredRotation, value));
}

void ASoldier::RotateToPlayer()
{
	if (TargetStimuliActors.IsEmpty()) return;
	FVector actorLocation = FVector(GetActorLocation().X, GetActorLocation().Y, 0);
	AActor* Target = nullptr;
	for (int i = 0; i < TargetStimuliActors.Num(); i++)
	{
		if (Cast<ABloodCharacter>(TargetStimuliActors[i]))
		{
			Target = TargetStimuliActors[i].Get();
		}
	}

	if (!Target) return;
	FRotator rotationVector = UKismetMathLibrary::FindLookAtRotation(actorLocation, FVector(Target->GetActorLocation().X, Target->GetActorLocation().Y, 0));

	SetActorRotation(rotationVector);
}

void ASoldier::OnPatrolPointReached(FAIRequestID requestID, const FPathFollowingResult& result)
{
	if (result.IsSuccess()) 
	{
		HasFinishedMovement = true;
		if (PatrolPath->GetPatrolPoint().WaitTime <= 0) 
		{
			EndPatrol();
		}
		else 
		{
			Rotate();
		}
		PatrolPath->SoldierReachedPoint(this);
	}
}

void ASoldier::OnInteractableObjectReached(FAIRequestID requestID, const FPathFollowingResult& result) 
{
	if (result.IsSuccess()) 
	{
		IsInteracting = true;
		HasFinishedMovement = true;
	}
}

void ASoldier::OnLastSeenLocationReached(FAIRequestID requestID, const FPathFollowingResult& result) 
{
		HasReachedLastSeenPosition = true;
		HasFinishedMovement = true;
		CanLoseDetectionSearch = true;
		TimelineLoseDetection.PlayFromStart();

		if (HasSeenDeadBody) 
		{
			HasSeenDeadBody = false;
			EndWaiting();
		}
		else 
		{
			GetWorldTimerManager().SetTimer(EndWaitOnSearchTimerHandle, this, &ASoldier::EndWaiting, SoldierDA->WaitTimeOnSearchLocation);
		}
		
}

void ASoldier::EndInteract()
{
	IIInteractable::Execute_Interact(ObjectToInteract.Get());
	EndBlind();
	if (GetGOAPController().IsValid())
	{
		actual_goal_map_key = "MAIN";
		ChangePlan();
	}
	IsInteracting = false;
	ObjectToInteract = nullptr;
}

void ASoldier::BiteSound_Implementation(bool IsSilent, bool PlayLongSound)
{
	UAkAudioEvent* SoldierSound;
	if (IsSilent)
	{
		if (PlayLongSound)
		{
			SoldierSound = SoundDataAsset->SilentLongBiteEvent;
		}
		else
		{
			SoldierSound = SoundDataAsset->SilentShortBiteEvent;
		}
	}
	else
	{
		UAISense_Hearing::ReportNoiseEvent(GetWorld(), this->GetActorLocation(), 1, this, SoldierDA->ShortBiteRange, FName(TEXT("Instant")));
		if (PlayLongSound)
		{
			SoldierSound = SoundDataAsset->LoudLongBiteEvent;
		}
		else
		{
			SoldierSound = SoundDataAsset->LoudShortBiteEvent;
		}
	}
	
	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->ChargeShootStopEvent, this, 0, NullCallback);
	UAkGameplayStatics::PostEvent(SoldierSound, this, 0, NullCallback);
}

void ASoldier::Revive_Implementation()
{
	IsDead = false;
	IsStunned = false;
	IsBeingBitten = false;
	DetectionPercentage = 0;
	
	if (PatrolPath)
		PatrolPath->SetCurrentPatrolPointIdx(0);

	EnemyDetectionWidget->SetDetectionPercentage(0);
	GetMesh()->InitAnim(true);
	EnemyDetectionWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	TargetStimuliActors.Empty();
	AIPerceptionComponentRename->ForgetAll();
	Possessed();
	EndBlind();
}

void ASoldier::Die_Implementation()
{
	IsDead = true;
	EnemyDetectionWidget->SetVisibility(ESlateVisibility::Hidden);
	CurrentHUD->RemoveEnemyMarker(this);

	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->BlindLoopStop, this, 0, NullCallback);
	
	UnPossessed();
}

void ASoldier::BeingAttacked_Implementation()
{
	GetMesh()->GetAnimInstance()->ProcessEvent(GetMesh()->GetAnimInstance()->FindFunction(FName("BeingAttacked")), NULL);
}

void ASoldier::BeingArialBite_Implementation()
{
	GetMesh()->GetAnimInstance()->ProcessEvent(GetMesh()->GetAnimInstance()->FindFunction(FName("GetAerialDead")), NULL);
}

void ASoldier::BeingBlind_Implementation(float f)
{
	Stun_Implementation(f);
}


void ASoldier::Stun_Implementation(float t)
{
	Stun_Event(t);
	IsStunned = true;
	//AGOAPController* goapController = Cast<AGOAPController>(GetController());
	if (GetGOAPController().IsValid())
	{
		GoapController->StopBehaviorTree();
		GoapController->StopMovement();
		GoapController->SetActualWorldStateAtomValue("EnemyVisible", false);
		AIPerceptionComponentRename->SetSenseEnabled(UAISense_CustomSight::StaticClass(), false);
		AIPerceptionComponentRename->SetSenseEnabled(UAISense_Hearing::StaticClass(), false);
		TargetStimuliActors.Empty();
		CanLoseDetectionHearing = true;
		CanLoseDetectionSearch = true;
		IsEnemyInSight = false;
		IsEnemyHeared = false;
		GetWorldTimerManager().ClearTimer(StunTimerHandle);
		GetWorldTimerManager().ClearTimer(EndPatrolTimerHandle);
		GetWorldTimerManager().ClearTimer(EndWaitOnSearchTimerHandle);
		TimelineDetection.Stop();
		//TimelineLoseDetection.PlayFromStart();
		GetWorldTimerManager().SetTimer(StunTimerHandle, this, &ASoldier::Unstun, t, false);

		const FOnAkPostEventCallback NullCallback;
		UAkGameplayStatics::PostEvent(SoundDataAsset->ChargeShootStopEvent, this, 0, NullCallback);
		UAkGameplayStatics::PostEvent(SoundDataAsset->BlindLoop, this, 0, NullCallback);
	}
}

void ASoldier::ActivateShield()
{
	IsShielded = true;
	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->ShieldActivationEvent, this, 0, NullCallback);
	ShieldEffect();
}

void ASoldier::DeactivateShield()
{
	IsShielded = false;
	const FOnAkPostEventCallback NullCallback;
	UAkGameplayStatics::PostEvent(SoundDataAsset->ShieldDeactivationEvent, this, 0, NullCallback);
	ShieldEffect();
}

void ASoldier::ChangeOutline(float value, FLinearColor color)
{
	if (value != 0 && !OutlineActivated) 
	{
		GetMesh()->SetCustomDepthStencilValue(GetMesh()->CustomDepthStencilValue + 2);
		OutlineActivated = true;
	}
	else if (value == 0 && OutlineActivated) 
	{
		GetMesh()->SetCustomDepthStencilValue(GetMesh()->CustomDepthStencilValue - 2);
		OutlineActivated = false;
	}
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), MPCSoldier, FName(TEXT("OutlineSeeThrough")), value);
	UKismetMaterialLibrary::SetVectorParameterValue(GetWorld(), MPCSoldier, FName(TEXT("OutlineColor")), color);
}

void ASoldier::Unstun() 
{
	IsStunned = false;
	//AGOAPController* goapController = Cast<AGOAPController>(GetController());
	if (GetGOAPController().IsValid())
	{
		Suspicious();
		TimelineLoseDetection.PlayFromStart();
		if (DetectionPercentage < 100)
		{
			DetectionPercentage = 100;
			CurrentHUD->AddEnemyMarker(this);
			//ChangePlan();
		}
		GoapController->RestartBehaviorTree();
		AIPerceptionComponentRename->SetSenseEnabled(UAISense_CustomSight::StaticClass(), true);
		AIPerceptionComponentRename->SetSenseEnabled(UAISense_Hearing::StaticClass(), true);
		const FOnAkPostEventCallback NullCallback;
		UAkGameplayStatics::PostEvent(SoundDataAsset->BlindLoopStop, this, 0, NullCallback);
		UAkGameplayStatics::PostEvent(SoundDataAsset->BlindEnd, this, 0, NullCallback);
	}
}

void ASoldier::Blind(float t) 
{
	Blind_Event(t);
	//AGOAPController* goapController = Cast<AGOAPController>(GetController());
	if (GetGOAPController().IsValid())
	{
		GoapController->StopBehaviorTree();
		GoapController->StopMovement();

		UE_LOG(LogTemp, Warning, TEXT("Blind"));
		GetWorldTimerManager().SetTimer(EndBlindTimerHandle, this, &ASoldier::EndBlind, t, false);
	}
}

void ASoldier::EndBlind()
{
	//AGOAPController* goapController = Cast<AGOAPController>(GetController());
	if (GetGOAPController().IsValid())
	{
		GoapController->SetActualWorldStateAtomValue("HasSeenEnemyRecently", true);
		if (DetectionPercentage < 100) 
		{
			DetectionPercentage = 100;
			CurrentHUD->AddEnemyMarker(this);
			ChangePlan();
		}
		GoapController->RestartBehaviorTree();
		
		TimelineLoseDetection.Stop();
	}
}

void ASoldier::AlertEnemies() 
{
	TArray<ASoldier*> foundEnemies;
	foundEnemies = AIManagerSubsystem->GetAllSoldiersInRange(this->GetActorLocation(), SoldierDA->AlertRange);

	for (TWeakObjectPtr<ASoldier> soldier : foundEnemies) 
	{
		if (soldier->GetDetectionPercentage() < 100) soldier->SetDetectionPercentage(100);
		if (GetGOAPController().IsValid() && soldier->GetDetectionPercentage() < 200)
		{
			soldier->GoapController->MoveToLocation(this->GetActorLocation(), 200);
			soldier->GoapController->SetActualWorldStateAtomValue("HasSeenEnemyRecently", true);
			EnemyDetectionWidget->SetHasSeenEnemyRecently(true);
			soldier->GoapController->SetActualWorldStateAtomValue("PlanStateChanged", true);
		}
	}
}

bool ASoldier::Interact()
{
	UPathFollowingComponent* pathFollowingComponent = GoapController->GetPathFollowingComponent();
	pathFollowingComponent->OnRequestFinished.RemoveAll(this);
	pathFollowingComponent->OnRequestFinished.AddUObject(this, &ASoldier::OnInteractableObjectReached);

	if (!ObjectToInteract.Get()) return false;

	Move(ObjectToInteract->GetActorLocation());

	return true;
}

void ASoldier::Use()
{
	if (!ObjectToInteract.Get()) return;
	IsInteracting = true;
	DisableInput(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

bool ASoldier::GetHasFinishedMovement() 
{
	return HasFinishedMovement;
}

bool ASoldier::GetHasEndedPatrol()
{
	return HasEndedPatrol;
}

bool ASoldier::GetHasEndedSearch() 
{
	return HasEndedSearch;
}

void ASoldier::SetCanStrikePlayer(bool b)
{
	StrikeUp = b;
}

bool ASoldier::GetHasEndedStrike()
{
	return HasEndedStrike;
}

TArray<TWeakObjectPtr<AActor>> ASoldier::GetTargetStimuliActors() 
{
	return TargetStimuliActors;
}

void ASoldier::FindPriorityTarget() 
{
	if (TargetStimuliActors.IsEmpty()) TargetIdx = -1;
}

bool ASoldier::TargetStimuliActorsContains(AActor* actor) 
{
	return TargetStimuliActors.Contains(actor);
}

float ASoldier::GetFireRange() 
{
	return SoldierDA->FireRange;
}

////// START PROTO MIND CONTROL //////
float ASoldier::GetDetectionPercentage() const
{
	return DetectionPercentage;
}

void ASoldier::SetDetectionPercentage(float detectionPercentage) 
{
	DetectionPercentage = detectionPercentage;
}
////// END PROTO MIND CONTROL //////

void ASoldier::SetObjectToInteract(TSoftObjectPtr<AActor> object) 
{
	ObjectToInteract = object.Get();
	if (GetGOAPController().IsValid()) 
	{
		if (ObjectToInteract.Get()) 
		{
			GoapController->SetActualWorldStateAtomValue("InteractableState", false);
		}
		else 
		{
			GoapController->SetActualWorldStateAtomValue("InteractableState", true);
		}
	}
}

TSoftObjectPtr<AActor> ASoldier::GetObjectToInteract() 
{
	return ObjectToInteract.Get();
}

void ASoldier::SetIsBeingBitten(bool b) 
{
	IsBeingBitten = b;
}

bool ASoldier::EnemyVisible() 
{
	if (GetGOAPController().IsValid()) return GoapController->BlackboardComp->GetValueAsBool("EnemyVisible");
	return false;
}

bool ASoldier::GetIsDead()
{
	return IsDead;
}

bool ASoldier::GetSoundProcessSFE()
{
	return SoundProcessSFE;
}

bool ASoldier::GetHasSeenEnemyRecently()
{
	if(GetGOAPController().IsValid()) return GetGOAPController().Get()->BlackboardComp->GetValueAsBool("HasSeenEnemyRecently");
	return false;
}

bool ASoldier::GetIsInEnemySight()
{
	return IsEnemyInSight;
}

bool ASoldier::GetIsEnemyHeared()
{
	return IsEnemyHeared;
}

bool ASoldier::GetIsShielded()
{
	return IsShielded;
}

float ASoldier::GetFitnessByRange(const FVector& Location) const
{
	return FVector::DistSquared(GetActorLocation(), Location);
}

BehaviorStates ASoldier::GetCurrentBehaviorState() 
{
	return CurrentState;
}

BehaviorStates ASoldier::GetLastBehaviorState() 
{
	return LastState;
}

void ASoldier::SetBehaviorStates(BehaviorStates NewState) 
{
	LastState = CurrentState;
	CurrentState = NewState;
}

void ASoldier::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	
	OutLocation = VisionPoint->GetComponentLocation();
	OutRotation = GetActorRotation();
}

FGenericTeamId ASoldier::GetGenericTeamId() const
{
	return TeamID;
}

void ASoldier::ForgetHearingStimuli(AActor* actor, bool b) 
{
	if (Cast<ABloodCharacter>(actor) && !canRemovePlayerStimuli) return;
	SoundProcessSFE = true;
	CanLoseDetectionTimerDelegate.Unbind();
	if(TargetStimuliActors.Contains(actor))
		TargetStimuliActors.Remove(actor);
	CanLoseDetectionHearing = b;
	TimelineLoseDetection.PlayFromStart();
}

void ASoldier::DelegateOnTargetPerceptionUpdated(AActor* actor, FAIStimulus stimulus) 
{
	if (!GetGOAPController().IsValid()) return;

	FName enemyVisibleKey = FName(TEXT("EnemyVisible"));
	FName lastSeenLocationKey = FName(TEXT("LastSeenLocation"));
	FName hasSeenEnemyRecentlyKey = FName(TEXT("HasSeenEnemyRecently"));
	if (stimulus.Type == IDSight) 
	{
		OnTargetSightPerceptionUpdated(stimulus, actor, lastSeenLocationKey);
	} 
	else if (stimulus.Type == IDHearing) 
	{
		OnTargetHearingPerceptionUpdated(stimulus, actor, lastSeenLocationKey);
	}
	EnemyDetectionWidget->SetDetectionPercentage(DetectionPercentage);
}

void ASoldier::OnTargetSightPerceptionUpdated(FAIStimulus& stimulus, AActor* actor, FName& lastSeenLocationKey)
{
	if (stimulus.IsActive())
	{
		if (GoapController->GetTeamAttitudeTowards(*actor) == ETeamAttitude::Hostile) 
		{
			EnterSight(actor);
		}
		else if (GoapController->GetTeamAttitudeTowards(*actor) == ETeamAttitude::Friendly) 
		{
			ASoldier* Soldier = Cast<ASoldier>(actor);
			if (Soldier)
			{
				if (FVector::DistSquaredXY(GetActorLocation(), Soldier->GetActorLocation()) < SoldierDA->CorpseDetectionRange * SoldierDA->CorpseDetectionRange)
				{
					//NE S'UPDATE QUE QUAND LE STIMULI EST VU POUR LA PREMIERE FOIS, ET PAS TOUT LE TEMPS
					FindCorpse(Soldier, actor, lastSeenLocationKey);
				}
			}
		}
	}
	else if (GoapController->GetTeamAttitudeTowards(*actor) == ETeamAttitude::Hostile)
	{
		ExitSight(actor, lastSeenLocationKey, stimulus);
	}
}

void ASoldier::FindCorpse(ASoldier* Soldier, AActor*& actor, FName& lastSeenLocationKey)
{
	if (Soldier->GetIsDead() && !AlreadyFoundCorpses.Contains(actor))
	{
		DialogueComponent->PlayDialogue(TriggerEventType::DeadCorpse);
		AlreadyFoundCorpses.AddUnique(actor);
		HasSeenDeadBody = true;
		if (!GoapController->BlackboardComp->GetValueAsBool("HasSeenEnemyRecently"))
		{
			GoapController->SetActualWorldStateAtomValue("HasSeenEnemyRecently", true);
			EnemyDetectionWidget->SetHasSeenEnemyRecently(true);
		}
		if (DetectionPercentage < 100) 
		{
			
			CurrentHUD->AddEnemyMarker(this);
			DetectionPercentage = 100;
		}
		
		if (!IsShielded)
			ActivateShield();

		GetWorldTimerManager().ClearTimer(LoseShieldTimerHandle);
		GetWorldTimerManager().SetTimer(LoseShieldTimerHandle, this, &ASoldier::DeactivateShield, SoldierDA->ShieldDuration);

		GoapController->BlackboardComp->SetValueAsVector(lastSeenLocationKey, Soldier->GetActorLocation());
		ChangePlan();
	}
	if (Soldier->IsBeingBitten && !AlreadyFoundCorpses.Contains(actor))
	{
		AlreadyFoundCorpses.AddUnique(actor);
	}
}

void ASoldier::EnterSight(AActor* actor)
{

	//_targetStimuliActor = actor;
	ABloodCharacter* BloodCharacter = Cast<ABloodCharacter>(actor);
	IsEnemyInSight = true;
	HasSeenDeadBody = false;
	EnemyDetectionWidget->SetDetectBySight(true);
	if (BloodCharacter)
	{
		canRemovePlayerStimuli = false;
		DetectionSpeedMultiplier = 1;
		float Dot = GetActorForwardVector().Dot(BloodCharacter->GetActorLocation() - GetActorLocation());
		if ( Dot > 0) 
		{
			DialogueComponent->PlayDialogue(TriggerEventType::EnterSight);
		}
	}
	TargetStimuliActors.AddUnique(actor);
	TimelineDetection.PlayFromStart();
	TimelineLoseDetection.Stop();
}

void ASoldier::ExitSight(AActor* actor, FName& lastSeenLocationKey, FAIStimulus& stimulus)
{
	IsEnemyInSight = false;
	EnemyDetectionWidget->SetDetectBySight(false);
	if (Cast<ABloodCharacter>(actor)) 
	{
		canRemovePlayerStimuli = true;
		UE_LOG(LogTemp, Warning, TEXT(""))
	}
	TargetStimuliActors.Remove(actor);
	
	GoapController->SetActualWorldStateAtomValue("EnemyVisible", false);
	GoapController->BlackboardComp->SetValueAsVector(lastSeenLocationKey, stimulus.StimulusLocation);
	if (TargetStimuliActors.IsEmpty())
	{
		TimelineLoseDetection.PlayFromStart();
		TimelineDetection.Stop();
	}
}

void ASoldier::RemoveStimuli() 
{
	AActor* Target = nullptr;
	for (int i = 0; i < TargetStimuliActors.Num(); i++)
	{
		if (Cast<ABloodCharacter>(TargetStimuliActors[i]))
		{
			Target = TargetStimuliActors[i].Get();
		}
	}
	if (!Target) return;
	TargetStimuliActors.Remove(Target);
}

void ASoldier::OnTargetHearingPerceptionUpdated(FAIStimulus& stimulus, AActor* actor, FName& lastSeenLocationKey) 
{
	if (stimulus.IsActive() && GoapController->GetTeamAttitudeTowards(*actor) == ETeamAttitude::Hostile || GoapController->GetTeamAttitudeTowards(*actor) == ETeamAttitude::Friendly) 
	{
		CanLoseDetectionTimerDelegate.BindUObject(this, &ASoldier::ForgetHearingStimuli, actor, true);
		CanLoseDetectionHearing = false;
		SoundProcessSFE = false;
		HasSeenDeadBody = false;
		GetWorld()->GetTimerManager().ClearTimer(CanLoseDetectionTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(CanLoseDetectionTimerHandle, CanLoseDetectionTimerDelegate, SoldierDA->ForgetSoundTimer, false);
		TargetStimuliActors.AddUnique(actor);
		GoapController.Get()->BlackboardComp->SetValueAsVector(lastSeenLocationKey, stimulus.StimulusLocation);

		if (DetectionPercentage < 100) 
		{
			if (stimulus.Tag == "Instant")
			{
				CurrentHUD->AddEnemyMarker(this);
				ChangeOutline(2, FLinearColor(1, 0.92, 0, 0));
				//DialogueComponent->PlayDialogue(TriggerEventType::ShootingSound);
				DetectionPercentage = 100;
			}
			else if (stimulus.Tag == "Incremental")
			{
				CurrentHUD->AddEnemyMarker(this);
				ChangeOutline(2, FLinearColor(1, 1, 1, 0));
				FVector toEnemy = actor->GetActorLocation() - GetActorLocation();
				float DetectionValue = GetWorld()->DeltaTimeSeconds * SoldierDA->CurveHearingStepDetectionSpeed->GetFloatValue((toEnemy.SquaredLength() / (AISenseConfigHearing->HearingRange * AISenseConfigHearing->HearingRange)));
				DetectionValue *= DetectionHearingMultiplier;
				DetectionPercentage += DetectionValue;
				if (!TimelineHearingDetection.IsPlaying()) 
				{
					TimelineHearingDetection.PlayFromStart();
				}
				IsEnemyHeared = true;
			}

			if (Cast<ABloodCharacter>(actor) != nullptr)
			{
				if (!VibrationThresholdActivated && DetectionPercentage >= SoldierDA->DetectionVibrationThreshold)
				{
					UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientPlayForceFeedback(VibrationDA->HearingDetection);
					VibrationThresholdActivated = true;
				}
			}
		}
		if (DetectionPercentage >= 100)
		{
			if (!GoapController.Get()->BlackboardComp->GetValueAsBool("HasSeenEnemyRecently")) 
			{
				WasSuspicious = true;
				SuspiciousEffect();
				ChangeOutline(2, FLinearColor(1, 0.92, 0, 0));
				DialogueComponent->PlayDialogue(TriggerEventType::SoundHeared);
				SetBehaviorStates(BehaviorStates::Suspect);
				GoapController.Get()->SetActualWorldStateAtomValue("HasSeenEnemyRecently", true);
				EnemyDetectionWidget->SetHasSeenEnemyRecently(true);
			}
		}

		
	}
}

void ASoldier::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->ImplementsInterface(UIInteractable::StaticClass()))
	{
		ObjectToInteract = OtherActor;
	}
}

void ASoldier::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, const class AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == ObjectToInteract)
	{
		ObjectToInteract = nullptr;
	}
}

