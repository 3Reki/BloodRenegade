
#include "W_HookViewport.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"


void UW_HookViewport::MovePointer(FVector2D NewPosition, float f)
{
	HookPointerBorder->SetRenderTranslation(FMath::Lerp(HookPointerBorder->GetRenderTransform().Translation, NewPosition, 0.18f));
	HookPointerPercent->SetRenderTranslation(HookPointerBorder->GetRenderTransform().Translation);
	HookCross->SetRenderTranslation(HookPointerBorder->GetRenderTransform().Translation);
	SetCursorPercent(f);
}

void UW_HookViewport::SetCursorPercent(float f)
{
	if(f<1 && HookPointerPercent->GetPercent()>=1)
	{
		HookPointerBorder->SetVisibility(ESlateVisibility::Visible);
		HookPointerPercent->SetVisibility(ESlateVisibility::Visible);
		HookPointerBorder->SetBrushFromTexture(HookAssetList[0]);
	}else if(f>=1 && HookPointerPercent->GetPercent()<=1)
	{
		HookPointerPercent->SetVisibility(ESlateVisibility::Hidden);
		HookPointerBorder->SetBrushFromTexture(HookAssetList[1]);
	}

	HookPointerPercent->SetPercent(f);
}

void UW_HookViewport::CantHook()
{
	HookCross->SetRenderTranslation(HookPointerBorder->GetRenderTransform().Translation);
	PlayAnimation(HookCrossAnim.Get());
}

void UW_HookViewport::SetActiveHook(bool b)
{
	if(b && HookPointerBorder->GetVisibility() == ESlateVisibility::Hidden)
	{
		if(HookPointerPercent->GetPercent()<1)
		{
			HookPointerPercent->SetVisibility(ESlateVisibility::Visible);
		}
		HookPointerBorder->SetVisibility(ESlateVisibility::Visible);
		return;
	}

	if(!b && HookPointerBorder->GetVisibility() == ESlateVisibility::Visible)
	{
		HookPointerPercent->SetVisibility(ESlateVisibility::Hidden);
		HookPointerBorder->SetVisibility(ESlateVisibility::Hidden);
	}
}
