#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AIManagerSubsystem.generated.h"

class ASoldier;

DECLARE_DELEGATE_OneParam(FOnAlertChangedSignature, bool /* IsAlerted */)

USTRUCT()
struct FSoldierInfos
{
	GENERATED_BODY()

	FVector InitialPosition;
	bool IsAlerted;
	bool IsSuspicious;
};

UCLASS()
class BLOODRENEGADE_API UAIManagerSubsystem : public UWorldSubsystem
{

	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	bool IsDetectionIndicatorVisible = true;

	FOnAlertChangedSignature OnAlertChangedDelegate;
	FOnAlertChangedSignature OnSuspiciousChangedDelegate;
	
	void AddSoldier(TWeakObjectPtr<ASoldier> Soldier);
	
	void ResetAllSoldiers();

	UFUNCTION(BlueprintCallable)
	void ResetSoldier(ASoldier* Soldier);

	UFUNCTION(BlueprintCallable)
	TSoftObjectPtr<ASoldier> FindAIByFitness(TSoftObjectPtr<AActor> AlertActor);

	UFUNCTION(BlueprintCallable)
	void ChangeGoal(FName GoalKey, ASoldier* AffectedSoldiers);

	UFUNCTION(BlueprintCallable)
	TArray<ASoldier*> GetAllSoldiersInRange(FVector Pos, float Range);


	void SetDetectionIndicatorVisible(bool Visible);

	UFUNCTION()
	void SetCanStrikePlayer(bool b);

	void SetSoldierIsAlerted(ASoldier* Soldier, bool IsAlerted);
	
	void SetSoldierIsSuspicious(ASoldier* Soldier, bool IsSuspicious);


protected:
	UPROPERTY(EditAnywhere, Category = "Default")
	TMap<TWeakObjectPtr<ASoldier>, FSoldierInfos> Soldiers {};

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

};
