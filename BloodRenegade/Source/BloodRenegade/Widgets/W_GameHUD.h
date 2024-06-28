// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "W_GameHUD.generated.h"

class URichTextBlock;
class UDataTable;
class UTextBlock;
class UProgressBar;
/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UW_GameHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* BloodSlot1;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* BloodSlot2;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* BloodSlot3;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* BloodSlot4;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* BloodSlot5;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* BloodSlot6;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* BloodSlot7;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* BloodSlot8;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* BloodSlot9;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* BloodSlot10;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* TargetBlind1;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* TargetBlind2;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* TargetBlind3;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* TargetBlind4;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* TargetBlind5;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* QTERange;

	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim),Transient)
	TWeakObjectPtr<UWidgetAnimation> QteFailed;

	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim),Transient)
	TWeakObjectPtr<UWidgetAnimation> QteSuceed;
	
	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim),Transient)
	TWeakObjectPtr<UWidgetAnimation> InputListAnim;

	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim),Transient)
	TWeakObjectPtr<UWidgetAnimation> ActiveNightmareFormAnim;

	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim),Transient)
	TWeakObjectPtr<UWidgetAnimation> ActiveBlood;

	UPROPERTY(EditAnywhere, meta = (BindWidgetAnim), Transient)
	TWeakObjectPtr<UWidgetAnimation> CantUseAnim;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UImage> BiteInput;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UImage> BiteCross;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UImage> InteractInput;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (BindWidget))
	TWeakObjectPtr<UImage> BiteFakeProgressBar;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<URichTextBlock> BiteInputText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	TWeakObjectPtr<UTextBlock> ObjectiveText;

	UPROPERTY(EditAnywhere)
	UDataTable* GamepadIconsDataTable;

	UPROPERTY(EditAnywhere)
	UDataTable* KeyboardIconsDataTable;

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetBiteProgressBarPercent(float f);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetBiteProgressBarColor(FColor Color);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetBiteProgressBarThickness(float f);

	UFUNCTION(BlueprintCallable)
	void RefreshInteractInput(FName InputName, bool UseControllerInput);

	UFUNCTION(BlueprintCallable)
	void RefreshBiteInput(FName InputName, bool UseControllerInput);

	void SetInteractInputLocation(FVector2d pos);
	void SetBiteInputLocation(FVector2d pos);
	void SetActiveInputInteract(bool b);
	void SetActiveInputBite(bool b);
	void SetBiteText(FString s);
	void SetActiveInputList(bool b);
	void SetActiveInputNF(bool b);
	void SetObjectiveText(const FString& ObjectiveName);
	void SetActiveTarget(int index, bool b);
	void SetTargetPos(int index, FVector2d pos);
	void SetScaleBiteBar(float f);
	void SetBiteTimingQTE(bool b);
	void SetBiteCross(bool b);
	void CantSkill();
	void InitializeBlood(int i);
	void UpdateCurrentBlood(int i, float value);
	void UpdateAllBlood(int i);
private:
	
	TArray<TWeakObjectPtr<UProgressBar>> ListBlood;
	TArray<TWeakObjectPtr<UImage>> ListTarget;
};
