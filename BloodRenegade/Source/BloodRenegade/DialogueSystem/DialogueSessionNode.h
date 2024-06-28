// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericGraphNode.h"
#include "Voices.h"
#include "AkAudioEvent.h"
#include "DialogueSessionNode.generated.h"


/**
 * 
 */
UCLASS()
class BLOODRENEGADE_API UDialogueSessionNode : public UGenericGraphNode
{
	GENERATED_BODY()
	
public:
    UDialogueSessionNode();

	UPROPERTY(EditAnywhere, Category = "Dialogue Node")
	FText Paragraph;

	UPROPERTY(EditAnywhere, Category = "Dialogue Node")
	TMap<UVoices, UAkAudioEvent*> DialogueLines;
	//USoundBase* DialogueLine;

	UPROPERTY(EditAnywhere, Category = "Dialogue Node")
	int SpeakerIdx;

	UPROPERTY(EditAnywhere, Category = "Dialogue Node")
	FString Subtitles;

	UPROPERTY(EditAnywhere, Category = "Dialogue Node")
	FLinearColor TextColor;

	UPROPERTY(EditAnywhere, Category = "Dialogue Node")
	float ActivationPercentage;

#if WITH_EDITOR
    virtual FText GetNodeTitle() const override;

    virtual void SetNodeTitle(const FText& NewTitle) override;

    virtual FLinearColor GetBackgroundColor() const override;
#endif
};
