// Fill out your copyright notice in the Description page of Project Settings.


#include "W_EnemyMarker.h"
#include "../AI/Soldier.h"
#include "Components/Image.h"
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"
#include "W_EnemyDetection.h"
#include "Components/ScaleBox.h"
#include "Kismet/GameplayStatics.h"
#include "StereoRendering.h"
#include "Math/MathFwd.h"
#include <Blueprint/SlateBlueprintLibrary.h>

void UW_EnemyMarker::Start() 
{
	ListIcons = TArray<TWeakObjectPtr<UScaleBox>>();

	ListIcons.Add(Icon1);
	ListIcons.Add(Icon2);
	ListIcons.Add(Icon3);
	ListIcons.Add(Icon4);
	ListIcons.Add(Icon5);
	ListIcons.Add(Icon6);
	ListIcons.Add(Icon7);
	ListIcons.Add(Icon8);
	ListIcons.Add(Icon9);
	ListIcons.Add(Icon10);
	ListIcons.Add(Icon11);
	ListIcons.Add(Icon12);
	ListIcons.Add(Icon13);
	ListIcons.Add(Icon14);
	ListIcons.Add(Icon15);
	ListIcons.Add(Icon16);
	ListIcons.Add(Icon17);
	ListIcons.Add(Icon18);
	ListIcons.Add(Icon19);
	ListIcons.Add(Icon20);

	for (int i = 0; i < ListIcons.Num(); i++)
	{
		ListIcons[i]->SetVisibility(ESlateVisibility::Hidden);
	}

	if (Curve) 
	{
		FOnTimelineFloat ProgressUpdateFunction;
		ProgressUpdateFunction.BindUFunction(this, FName("UpdateWidget"));
		TimelineUpdate.AddInterpFloat(Curve.Get(), ProgressUpdateFunction);
		TimelineUpdate.SetLooping(true);
	}
}

void UW_EnemyMarker::NativeTick(const FGeometry& MyGeometry, float DeltaTime) 
{
	TimelineUpdate.TickTimeline(DeltaTime);
	if (!ListSoldiers.IsEmpty()) 
	{
		UpdateWidget();
	}
}

void UW_EnemyMarker::UpdateWidget() 
{
	FVector soldierLocation;
	FVector2D screenLocation;
	FVector2D clampVector;
	FVector2D resolutionVector;

	bool isBehindScreen = false;

	int viewportSizeX;
	int viewportSizeY;
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize( /*out*/ViewportSize);
	}
	else 
	{
		return;
	}

	GEngine->GameViewport->GetViewportSize(resolutionVector);

	viewportSizeX = GSystemResolution.ResX;
	viewportSizeY = GSystemResolution.ResY;

	FVector2D centerToPoint;

	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APlayerCameraManager* camera = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	FVector cameraForwardXY = FVector(camera->GetActorForwardVector().X, camera->GetActorForwardVector().Y, 0);
	cameraForwardXY.Normalize();

	for (int i = 0; i < ListIcons.Num() && i < ListSoldiers.Num(); i++)
	{
		//----TEMPORARY FIX----
		if (!ListSoldiers[i].IsValid())
		{
			ListSoldiers.RemoveAt(i);
			ListIcons[i]->SetVisibility(ESlateVisibility::Hidden);
			break;
		}
		///--------------------
		//----UPDATE DETECTION WIDGET----
		UW_EnemyDetection* EDWidget = Cast<UW_EnemyDetection>(ListIcons[i].Get()->GetChildAt(0));
		EDWidget->SetDetectionPercentage(ListSoldiers[i]->GetDetectionPercentage());
		EDWidget->SetDetectBySight(ListSoldiers[i]->GetIsInEnemySight());
		EDWidget->SetDetectByHearing(ListSoldiers[i]->GetIsEnemyHeared());
		EDWidget->SetHasSeenEnemyRecently(ListSoldiers[i]->GetHasSeenEnemyRecently());
		EDWidget->SetIsShooting(ListSoldiers[i]->GetIsInEnemySight() && ListSoldiers[i]->EnemyVisible() && ListSoldiers[i]->InEnemyRange());
		EDWidget->SetIsShielded(ListSoldiers[i]->GetIsShielded());
		ListIcons[i]->SetVisibility(ESlateVisibility::Visible);
		soldierLocation = ListSoldiers[i]->UIAnchorPoint->GetComponentLocation();
		FVector normalizedVector = (soldierLocation - camera->GetCameraLocation());
		normalizedVector.Normalize();

		
		double angle = FMath::RadiansToDegrees(FMath::Acos(camera->GetActorForwardVector().Dot(normalizedVector)));

		if (angle > camera->GetFOVAngle()/ 2) //Camera FOV par rapport au vector forward de la camera
		{
			clampVector.X = viewportSizeX - (GetDesiredSize().X);
			clampVector.Y = viewportSizeY - (GetDesiredSize().Y);

			float CrossProduct = FVector2D::CrossProduct(FVector2D(cameraForwardXY.X, cameraForwardXY.Y), FVector2D(normalizedVector.X, normalizedVector.Y));
			float Sign = FMath::Sign(CrossProduct);

			normalizedVector.Z = 0;
			normalizedVector.Normalize();
			angle = FMath::Acos(cameraForwardXY.Dot(normalizedVector));

			centerToPoint.X = sin(camera->GetActorUpVector().Dot(normalizedVector));
			centerToPoint.Y = Sign > 0 ? sin(angle) : -sin(angle);
			EDWidget->SetArrowAngle(FMath::RadiansToDegrees(FMath::Atan2(-centerToPoint.Y, -centerToPoint.X)));
			centerToPoint = centerToPoint.GetRotated(-90);
			centerToPoint.Normalize();
			
			
			//centerToPoint.X = Sign > 0 ? (centerToPoint.X * viewportSizeX/1.5) + viewportSizeX / 2 : viewportSizeX/2 - (centerToPoint.X * viewportSizeX/1.5);
			centerToPoint.X = (centerToPoint.X * (viewportSizeX)) + (viewportSizeX / 2);
			centerToPoint.Y = (centerToPoint.Y * (viewportSizeY)) + (viewportSizeY / 2);

			centerToPoint.X = FMath::Clamp(centerToPoint.X, Offset.X, clampVector.X - Offset.X);
			centerToPoint.Y = FMath::Clamp(centerToPoint.Y, Offset.Y, clampVector.Y - Offset.Y);
			//screenLocation.Y = FMath::GetMappedRangeValueClamped(FVector2D(0, clampVector.Y), FVector2D(viewportSizeY / 2, clampVector.Y), screenLocation.Y) - viewportSizeY/2;
			ListIcons[i]->SetRenderTranslation(centerToPoint);

			EDWidget->SetArrowVisible(true);
			
		}
		else
		{
			ProjectWorldToWidget(GetWorld()->GetFirstPlayerController(), soldierLocation, screenLocation, isBehindScreen, true);

			ListIcons[i]->SetRenderTranslation(screenLocation);
			EDWidget->SetArrowVisible(false);
		}
		
	}
}

void UW_EnemyMarker::AddSoldier(TSoftObjectPtr<ASoldier> soldier) 
{
	if (ListSoldiers.Num() == 0) 
	{
		TimelineUpdate.PlayFromStart();
	}
	int idx = ListSoldiers.AddUnique(soldier.Get());
	ListIcons[idx]->SetVisibility(ESlateVisibility::Visible);
}

void UW_EnemyMarker::RemoveSoldier(TSoftObjectPtr<ASoldier> soldier) 
{ 
	ListSoldiers.Remove(soldier.Get());
	for (int i = ListSoldiers.Num(); i < ListIcons.Num(); i++) 
	{
		ListIcons[i]->SetVisibility(ESlateVisibility::Hidden);
	}
	
	if (ListSoldiers.Num() == 0) 
	{
		TimelineUpdate.Stop();
	}
}

bool UW_EnemyMarker::ProjectToScreen(const TSoftObjectPtr<APlayerController> Player, const FVector& WorldPosition, FVector& ScreenPosition, bool& isBehindScreen, bool bPlayerViewportRelative)
{
	FVector Projected;
	bool bSuccess = false;
	isBehindScreen = false;
	FVector2D ScreenPosition2D;

	ULocalPlayer* const LP = Player ? Player->GetLocalPlayer() : nullptr;
	if (LP && LP->ViewportClient)
	{
		// get the projection data
		FSceneViewProjectionData ProjectionData;
		if (LP->GetProjectionData(LP->ViewportClient->Viewport, /*out*/ ProjectionData))
		{
			
			const FMatrix ViewProjectionMatrix = ProjectionData.ComputeViewProjectionMatrix();
			const FIntRect ViewRectangle = ProjectionData.GetConstrainedViewRect();

			FPlane Result = ViewProjectionMatrix.TransformFVector4(FVector4(WorldPosition, 1.f));
			if (Result.W < 0) isBehindScreen = true;
			if (Result.W == 0.f) { Result.W = 1.f; } 

			const float RHW = 1.f / FMath::Abs(Result.W);
			Projected = FVector(Result.X, Result.Y, Result.Z) * RHW;

			// Normalize to 0..1 UI Space
			const float NormX = (Projected.X / 2.f) + 0.5f;
			const float NormY = 1.f - (Projected.Y / 2.f) - 0.5f;

			Projected.X = (float)ViewRectangle.Min.X + (NormX * (float)ViewRectangle.Width());
			Projected.Y = (float)ViewRectangle.Min.Y + (NormY * (float)ViewRectangle.Height());

			bSuccess = true;
			ScreenPosition2D = FVector2D(Projected.X, Projected.Y);

			if (bPlayerViewportRelative)
			{
				ScreenPosition2D -= FVector2D(ProjectionData.GetConstrainedViewRect().Min);
			}

			ScreenPosition = FVector(ScreenPosition2D.X, ScreenPosition2D.Y, FVector::Dist(ProjectionData.ViewOrigin, WorldPosition));
			bSuccess = true;
		}
		else
		{
			ScreenPosition = FVector(1234, 5678, 0);
		}
	}
	return bSuccess;
}

bool UW_EnemyMarker::ProjectWorldToWidget(TSoftObjectPtr<APlayerController> PlayerController, FVector WorldLocation, FVector2D& ViewportPosition, bool& isBehindScreen, bool bPlayerViewportRelative)
{
	FVector ScreenPosition3D;
	bool bSuccess;

	if (PlayerController)
	{
		FVector PixelLocation;
		const bool bProjected = ProjectToScreen(PlayerController, WorldLocation, PixelLocation, isBehindScreen, bPlayerViewportRelative);

		if (bProjected)
		{
			FVector2D ScreenPosition(FMath::RoundToInt(PixelLocation.X), FMath::RoundToInt(PixelLocation.Y));

			FVector2D ViewportPosition2D;
			USlateBlueprintLibrary::ScreenToViewport(PlayerController.Get(), ScreenPosition, ViewportPosition2D);
			ScreenPosition3D.X = ViewportPosition2D.X;
			ScreenPosition3D.Y = ViewportPosition2D.Y;
			ScreenPosition3D.Z = PixelLocation.Z;

			bSuccess = true;
		}
	}
	else 
	{
		ScreenPosition3D = FVector::ZeroVector;
	}
	

	bSuccess = false;
	ViewportPosition = FVector2D(ScreenPosition3D.X, ScreenPosition3D.Y);
	return bSuccess;
}