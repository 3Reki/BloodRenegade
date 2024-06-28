// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TimelineComponent.h"
#include "W_EnemyMarker.generated.h"


class UImage;
class ASoldier;
class UCanvasPanelSlot;
class UCurveFloat;
class UW_EnemyDetection;
class UScaleBox;
/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UW_EnemyMarker : public UUserWidget
{
	GENERATED_BODY()

private:


protected:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon1;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon2;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon3;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon4;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon5;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon6;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon7;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon8;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon9;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon10;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon11;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon12;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon13;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon14;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon15;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon16;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon17;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon18;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon19;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UScaleBox> Icon20;

	UPROPERTY(EditAnywhere)
	FVector2D Offset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UCurveFloat> Curve;

	UPROPERTY()
	FTimeline TimelineUpdate;
public:

	UPROPERTY(EditAnywhere)
	TArray<TWeakObjectPtr<UScaleBox>> ListIcons = TArray<TWeakObjectPtr<UScaleBox>>();

	TArray<TWeakObjectPtr<ASoldier>> ListSoldiers = TArray<TWeakObjectPtr<ASoldier>>();

	void Start();

	void NativeTick(const FGeometry& MyGeometry, float deltaTime) override;

	UFUNCTION(BlueprintCallable)
	void UpdateWidget();
	
	UFUNCTION(BlueprintCallable)
	void AddSoldier(TSoftObjectPtr<ASoldier> soldier);

	UFUNCTION(BlueprintCallable)
	void RemoveSoldier(TSoftObjectPtr<ASoldier> soldier);

	UFUNCTION(BlueprintCallable)
	bool ProjectToScreen(const TSoftObjectPtr<APlayerController> Player, const FVector& WorldPosition, FVector& ScreenPosition, bool& isBehindScreen, bool bPlayerViewportRelative);

	UFUNCTION(BlueprintCallable)
	bool ProjectWorldToWidget(TSoftObjectPtr<APlayerController> PlayerController, FVector WorldLocation, FVector2D& ViewportPosition, bool& isBehindScreen, bool bPlayerViewportRelative);
};
