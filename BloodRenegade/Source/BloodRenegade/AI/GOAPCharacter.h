// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GOAP.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GOAPCharacter.generated.h"

UCLASS()
class BLOODRENEGADE_API AGOAPCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AGOAPCharacter();
	//AGOAPCharacter(const class FObjectInitializer& ObjectInitializer); //another Constructor we should use ?

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* BehaviorTree;

	TMap<FName, const char> ActionNames = TMap<FName, const char>();
	TMap<FName, int32> ActionValues = TMap<FName, int32>();
	TMap<FName, const char> ImportantAtoms = TMap<FName, const char>();
	TMap<FName, TArray<TTuple<FName, bool>>> Goals;
	FName actual_goal_map_key;
	FName last_goal_map_key;

	virtual void InitializeEntity() PURE_VIRTUAL(AGOAPCharacter::InitializeEntity, return;);
	virtual void InitializeActions() PURE_VIRTUAL(AGOAPCharacter::InitializeActions, return;);
	virtual void InitializeImportantAtoms() PURE_VIRTUAL(AGOAPCharacter::InitializeImportantAtoms, return;);
	virtual void InitializeGoals() PURE_VIRTUAL(AGOAPCharacter::InitializeGoals, return;);
	virtual void InitializeActionsCosts() PURE_VIRTUAL(AGOAPCharacter::InitializeActionsCosts, return;);
	virtual void CalculateActionsCosts() PURE_VIRTUAL(AGOAPCharacter::CalculateActionsCosts, return;);
	virtual void CheckGoals() PURE_VIRTUAL(AGOAPCharacter::CheckGoals, return;);
	virtual void ChangePlan() PURE_VIRTUAL(AGOAPCharacter::ChangePlan, return;);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	
};
