// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "FearSubsystem.generated.h"


DECLARE_DELEGATE_OneParam(FOnFearChangedSignature, float /* Fear */)

UCLASS()
class BLOODRENEGADE_API UFearSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	FOnFearChangedSignature OnFearChangedDelegate;

protected:


private:


};
