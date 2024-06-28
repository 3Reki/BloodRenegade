// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class TriggerEventType : uint8
{
	None,
	DeadCorpse,
	LampBlinded,
	ShootingSound,
	Blind,
	Decoy,
	EnterSight,
	SoundHeared
};
