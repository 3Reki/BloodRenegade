// Fill out your copyright notice in the Description page of Project Settings.


#include "BloodHUD.h"

#include "BloodCharacterDataAsset.h"
#include "../Widgets/W_EnemyMarker.h"
#include "../AI/Soldier.h"
#include "BloodRenegade/Widgets/W_Aim.h"
#include "BloodRenegade/Widgets/W_GameHUD.h"
#include "BloodRenegade/Widgets/W_PauseMenu.h"
#include "Blueprint/UserWidget.h"


void ABloodHUD::BeginPlay()
{
	Super::BeginPlay();

	IsVisible = true;
	
	WidgetHook = CreateWidget<UW_HookViewport>(GetOwningPlayerController(), WidgetHookClass);
	WidgetAim = CreateWidget<UW_Aim>(GetOwningPlayerController(), WidgetAimClass);
	WidgetGameHUD = CreateWidget<UW_GameHUD>(GetOwningPlayerController(), WidgetGameClass);
	WidgetPause = CreateWidget<UW_PauseMenu>(GetOwningPlayerController(), WidgetPauseClass);
	WidgetEnemyMarker = CreateWidget<UW_EnemyMarker>(GetOwningPlayerController(), WidgetEnemyMarkerClass);
	InitHUD();
	
	WidgetAim->SetRenderOpacity(0);
	WidgetAim->AddToViewport();
	WidgetHook->AddToViewport();
	WidgetGameHUD->AddToViewport();
	WidgetEnemyMarker->Start();
	WidgetGameHUD->InitializeBlood(DataAsset->MaxBloodGauge);
	WidgetEnemyMarker->AddToViewport();
}

void ABloodHUD::SetActiveAim(bool b)
{
	if(b)
	{
		WidgetAim->SetRenderOpacity(1);
	}else
	{
		WidgetAim->SetRenderOpacity(0);
	}
}

void ABloodHUD::TargetInRangeAIM(bool b)
{
	WidgetAim->TargetInRange(b);
}

void ABloodHUD::InitHUD()
{
	WidgetGameHUD->SetObjectiveText(CurrentObjective);
}

void ABloodHUD::AddEnemyMarker(ASoldier* Soldier) 
{
	WidgetEnemyMarker->AddSoldier(Soldier);
}

void ABloodHUD::RemoveEnemyMarker(ASoldier* Soldier) 
{
	WidgetEnemyMarker->RemoveSoldier(Soldier);
}

void ABloodHUD::UpdateMarkers() 
{
	WidgetEnemyMarker->UpdateWidget();
}

void ABloodHUD::SetActivePauseMenu(bool b)
{
	if(b)
	{
		WidgetPause->AddToViewport();
		return;
	}

	WidgetPause->RemoveFromParent();
}

void ABloodHUD::UpdateHookPosition(FVector2D NewPosition,float dist)
{
	WidgetHook->MovePointer(NewPosition, dist);
}

void ABloodHUD::SetActiveHookWidget(bool b)
{
	WidgetHook->SetActiveHook(b);
}

void ABloodHUD::SetBiteInputPos(FVector2d pos)
{
	WidgetGameHUD->SetBiteInputLocation(pos);
}

void ABloodHUD::SetInteractInputPos(FVector2d pos)
{
	WidgetGameHUD->SetInteractInputLocation(pos);
}

void ABloodHUD::SetActiveInteractInput(bool b)
{
	WidgetGameHUD->SetActiveInputInteract(b);
}

void ABloodHUD::SetActiveBiteInput(bool b)
{
	WidgetGameHUD->SetActiveInputBite(b);
}

void ABloodHUD::SetActiveQTERange(bool b)
{
	if(b)
	{
		WidgetGameHUD->QTERange->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	WidgetGameHUD->QTERange->SetVisibility(ESlateVisibility::Hidden);
}

void ABloodHUD::SetActiveHUD(const bool IsActive)
{
	IsVisible = IsActive;
	
	if (IsActive)
	{
		WidgetHook->SetVisibility(ESlateVisibility::Visible);
		WidgetAim->SetVisibility(ESlateVisibility::Visible);
		WidgetGameHUD->SetVisibility(ESlateVisibility::Visible);
		WidgetPause->SetVisibility(ESlateVisibility::Visible);
		WidgetEnemyMarker->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		WidgetHook->SetVisibility(ESlateVisibility::Hidden);
		WidgetAim->SetVisibility(ESlateVisibility::Hidden);
		WidgetGameHUD->SetVisibility(ESlateVisibility::Hidden);
		WidgetPause->SetVisibility(ESlateVisibility::Hidden);
		WidgetEnemyMarker->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABloodHUD::CantHook()
{
	WidgetHook->CantHook();
}

void ABloodHUD::CantSkill()
{
	WidgetGameHUD->CantSkill();
}

void ABloodHUD::QteSuccedAnimation(bool b)
{
	if(b)
	{
		WidgetGameHUD->PlayAnimation(WidgetGameHUD->QteSuceed.Get());
	}else
	{
		WidgetGameHUD->PlayAnimation(WidgetGameHUD->QteFailed.Get());
	}
}

void ABloodHUD::SetPercentBiteHold(float f)
{
	WidgetGameHUD->SetBiteProgressBarPercent(f);
}

void ABloodHUD::SetColorBiteHold(FColor f)
{
	WidgetGameHUD->SetBiteProgressBarColor(f);
}

void ABloodHUD::SetThicknessBiteHold(float f)
{
	WidgetGameHUD->SetBiteProgressBarThickness(f);
}

void ABloodHUD::SetBiteText(FString Text)
{
	WidgetGameHUD->SetBiteText(Text);
}

void ABloodHUD::SetActiveInputList(bool b)
{
	//WidgetGameHUD->SetActiveInputList(b);
}

void ABloodHUD::SetActiveNFInput(bool b)
{
	WidgetGameHUD->SetActiveInputNF(b);
}

void ABloodHUD::UpdateCurrentBlood(int i, float value)
{
	WidgetGameHUD->UpdateCurrentBlood(i, value);
}

void ABloodHUD::UpdateAllBlood(int i)
{
	WidgetGameHUD->UpdateAllBlood(i);
}

void ABloodHUD::SetActiveTarget(int index, bool b)
{
	WidgetGameHUD->SetActiveTarget(index, b);
}

void ABloodHUD::SetTargetPos(int index, FVector2d pos)
{
	WidgetGameHUD->SetTargetPos(index, pos);
}

void ABloodHUD::SetBiteBarScale(float f)
{
	WidgetGameHUD->SetScaleBiteBar(f);
}

void ABloodHUD::SetBiteQteTiming(bool b)
{
	WidgetGameHUD->SetBiteTimingQTE(b);
}

void ABloodHUD::SetQteRangeOpacity(float f)
{
	WidgetGameHUD->QTERange->SetRenderOpacity(f);
}

void ABloodHUD::SetPercentCrossHair(float f)
{
	WidgetAim->SetPercentCrossAir(f);
}

void ABloodHUD::SetActiveBlood(bool b)
{
	if(b)
	{
		WidgetGameHUD->PlayAnimation(WidgetGameHUD->ActiveBlood.Get());
	}else
	{
		WidgetGameHUD->PlayAnimationReverse(WidgetGameHUD->ActiveBlood.Get());
	}
}

void ABloodHUD::SetActiveBiteCross(bool b)
{
	WidgetGameHUD->SetBiteCross(b);
}

void ABloodHUD::SetObjectiveText(const FString ObjectiveName)
{
	CurrentObjective = ObjectiveName;
	
	if (!HasActorBegunPlay())
	{
		UE_LOG(LogTemp, Display, TEXT("Set Objective Text called before BeginPlay"));
		return;
	}
	
	WidgetGameHUD->SetObjectiveText(ObjectiveName);
}


