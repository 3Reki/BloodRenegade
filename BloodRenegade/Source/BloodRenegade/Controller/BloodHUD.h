// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BloodRenegade/Widgets/W_HookViewport.h"
#include "GameFramework/HUD.h"
#include "BloodHUD.generated.h"

class UW_QTE;
class UW_Skills;
class UBloodCharacterDataAsset;
class UW_BiteQTE;
class UW_Aim;
class UW_GameHUD;
class UW_PauseMenu;
class UW_EnemyMarker;
class ASoldier;

UCLASS()
class BLOODRENEGADE_API ABloodHUD : public AHUD
{
	GENERATED_BODY()

public:
	bool IsVisible;

	UPROPERTY(BlueprintReadOnly)
	FString CurrentObjective = "None";
	
	void SetActiveAim(bool b);
	void TargetInRangeAIM(bool b);
	void InitHUD();
	void AddEnemyMarker(ASoldier* Soldier);
	void RemoveEnemyMarker(ASoldier* Soldier);
	void UpdateMarkers();
	void SetActivePauseMenu(bool b);
	void UpdateHookPosition(FVector2D NewPosition,float dist);
	void SetActiveHookWidget(bool b);
	void SetBiteInputPos(FVector2d pos);
	void SetInteractInputPos(FVector2d pos);
	void SetActiveInteractInput(bool b);
	void SetActiveBiteInput(bool b);
	void SetActiveQTERange(bool b);
	void CantHook();
	void CantSkill();
	void QteSuccedAnimation(bool b);
	void SetPercentBiteHold(float f);
	void SetColorBiteHold(FColor f);
	void SetThicknessBiteHold(float f);
	void SetBiteText(FString Text);
	void SetActiveInputList(bool b);
	void SetActiveNFInput(bool b);
	void UpdateCurrentBlood(int i, float value);
	void UpdateAllBlood(int i);
	void SetActiveTarget(int index, bool b);
	void SetTargetPos(int index, FVector2d pos);
	void SetBiteBarScale(float f);
	void SetBiteQteTiming(bool b);
	void SetQteRangeOpacity(float f);
	void SetPercentCrossHair(float f);
	void SetActiveBlood(bool b);
	void SetActiveBiteCross(bool b);

	UFUNCTION(BlueprintCallable)
	void SetActiveHUD(bool IsActive);
	
	UFUNCTION(BlueprintCallable)
	void SetObjectiveText(FString ObjectiveName);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Widget)
	TSubclassOf<UW_Aim> WidgetAimClass;

	UPROPERTY()
	UW_Aim* WidgetAim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Widget)
	TSubclassOf<UW_GameHUD> WidgetGameClass;

	UPROPERTY()
	UW_GameHUD* WidgetGameHUD;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Widget)
	TSubclassOf<UW_PauseMenu> WidgetPauseClass;

	UPROPERTY()
	UW_PauseMenu* WidgetPause;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Variables|Data", meta = (AllowPrivateAccess = "true"))
	UBloodCharacterDataAsset* DataAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Widget)
	TSubclassOf<UW_EnemyMarker> WidgetEnemyMarkerClass;

	UPROPERTY()
	UW_EnemyMarker* WidgetEnemyMarker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Widget)
	TSubclassOf<UW_HookViewport> WidgetHookClass;

	UPROPERTY()
	UW_HookViewport* WidgetHook;
	
	virtual void BeginPlay() override;
};
