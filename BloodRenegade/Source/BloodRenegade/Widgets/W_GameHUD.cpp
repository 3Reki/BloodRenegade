// Fill out your copyright notice in the Description page of Project Settings.


#include "W_GameHUD.h"
#include "BloodRenegade/Controller/BloodHUD.h"
#include "Components/ProgressBar.h"
#include "Components/RichTextBlockImageDecorator.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Kismet/DataTableFunctionLibrary.h"


void UW_GameHUD::RefreshInteractInput(const FName InputName, const bool UseControllerInput)
{
	FRichImageRow* RichImageRow = new FRichImageRow;
	const UDataTable* DeviceDataTable = UseControllerInput ? GamepadIconsDataTable : KeyboardIconsDataTable;
	UDataTableFunctionLibrary::Generic_GetDataTableRowFromName(DeviceDataTable, InputName, RichImageRow);
	InteractInput->SetBrush(RichImageRow->Brush);
	delete RichImageRow;
}

void UW_GameHUD::RefreshBiteInput(const FName InputName, const bool UseControllerInput)
{
	FRichImageRow* RichImageRow = new FRichImageRow;
	const UDataTable* DeviceDataTable = UseControllerInput ? GamepadIconsDataTable : KeyboardIconsDataTable;
	UDataTableFunctionLibrary::Generic_GetDataTableRowFromName(DeviceDataTable, InputName, RichImageRow);
	BiteInput->SetBrush(RichImageRow->Brush);
	delete RichImageRow;
}

void UW_GameHUD::SetInteractInputLocation(FVector2d pos)
{
	InteractInput->SetRenderTranslation(pos);
}

void UW_GameHUD::SetBiteInputLocation(FVector2d pos)
{
	BiteFakeProgressBar->SetRenderTranslation(pos+FVector2D::UnitX()*100);
	BiteInput->SetRenderTranslation(pos+FVector2D::UnitX()*100);
	QTERange->SetRenderTranslation(pos+FVector2D::UnitX()*100);
	BiteInputText->SetRenderTranslation(pos+FVector2D::UnitX()*200);
	BiteCross->SetRenderTranslation(pos+FVector2D::UnitX()*100);
}

void UW_GameHUD::SetActiveInputInteract(bool b)
{
	if(b)
	{
		InteractInput->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	InteractInput->SetVisibility(ESlateVisibility::Hidden);
}

void UW_GameHUD::SetActiveInputBite(bool b)
{
	if(b)
	{
		BiteFakeProgressBar->SetVisibility(ESlateVisibility::Visible);
		BiteInputText->SetVisibility(ESlateVisibility::Visible);
		BiteInput->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	QTERange->SetVisibility(ESlateVisibility::Hidden);
	BiteFakeProgressBar->SetVisibility(ESlateVisibility::Hidden);
	BiteInput->SetVisibility(ESlateVisibility::Hidden);
	BiteInputText->SetVisibility(ESlateVisibility::Hidden);
}


void UW_GameHUD::SetBiteText(FString s)
{
	BiteInputText->SetText(FText::FromString(s));
}

void UW_GameHUD::SetActiveInputList(bool b)
{
	if(b)
	{
		PlayAnimation(InputListAnim.Get());
		return;	
	}

	PlayAnimationReverse(InputListAnim.Get());
}

void UW_GameHUD::SetActiveInputNF(bool b)
{
	if(b)
	{
		PlayAnimation(ActiveNightmareFormAnim.Get());
		return;	
	}

	PlayAnimationReverse(ActiveNightmareFormAnim.Get());
}

void UW_GameHUD::SetObjectiveText(const FString& ObjectiveName)
{
	ObjectiveText->SetText(FText::FromString(ObjectiveName));
}

void UW_GameHUD::SetActiveTarget(int index, bool b)
{
	if(b)
	{
		ListTarget[index]->SetVisibility(ESlateVisibility::Visible);
	}else
	{
		ListTarget[index]->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UW_GameHUD::SetTargetPos(int index, FVector2d pos)
{
	ListTarget[index]->SetRenderTranslation(pos);
}

void UW_GameHUD::SetScaleBiteBar(float f)
{
	BiteFakeProgressBar->SetRenderScale(FVector2d::One()*f);
}

void UW_GameHUD::SetBiteTimingQTE(bool b)
{
	if(b)
	{
		BiteInput->SetColorAndOpacity(FLinearColor::Yellow);
	}else
	{
		BiteInput->SetColorAndOpacity(FLinearColor::White);
	}
}

void UW_GameHUD::SetBiteCross(bool b)
{
	if(b && BiteCross->GetVisibility() == ESlateVisibility::Hidden)
	{
		BiteCross->SetVisibility(ESlateVisibility::Visible);
	}else if(!b && BiteCross->GetVisibility() == ESlateVisibility::Visible)
	{
		BiteCross->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UW_GameHUD::CantSkill()
{
	PlayAnimation(CantUseAnim.Get());
}

void UW_GameHUD::InitializeBlood(int i)
{
	ListBlood.Add(BloodSlot1);
	ListBlood.Add(BloodSlot2);
	ListBlood.Add(BloodSlot3);
	ListBlood.Add(BloodSlot4);
	ListBlood.Add(BloodSlot5);
	ListBlood.Add(BloodSlot6);
	ListBlood.Add(BloodSlot7);
	ListBlood.Add(BloodSlot8);
	ListBlood.Add(BloodSlot9);
	ListBlood.Add(BloodSlot10);

	ListTarget.Add(TargetBlind1);
	ListTarget.Add(TargetBlind2);
	ListTarget.Add(TargetBlind3);
	ListTarget.Add(TargetBlind4);
	ListTarget.Add(TargetBlind5);

	for (int j = 9;j>=0;j--){
		if(j>=i)
		{
			ListBlood[j]->SetRenderOpacity(0);
			ListBlood.RemoveAt(j);
		}else
		{
			ListBlood[j]->SetPercent(1);
		}
	}
}

void UW_GameHUD::UpdateCurrentBlood(int index, float value)
{
	ListBlood[index]->SetPercent(value);
}

void UW_GameHUD::UpdateAllBlood(int i)
{
	for (int j = 0; j<ListBlood.Num(); j++)
	{
		if(j<i)
		{
			ListBlood[j]->SetPercent(1);
		}else
		{
			ListBlood[j]->SetPercent(0);
		}
	}
}
