#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "BloodCheatManager.generated.h"

class UInputMappingContext;

UCLASS()
class BLOODRENEGADE_API UBloodCheatManager : public UCheatManager
{
	GENERATED_BODY()

public:
	virtual void Ghost() override;

	UFUNCTION(Exec)
	void ToggleSkillsCooldown() const;

	UFUNCTION(Exec)
	void SetHP(int Value) const;
	
	UFUNCTION(Exec, Category="MainCharacter")
	void SetMaxHP(int Value) const;

	UFUNCTION(Exec)
	void SetFear(float Value) const;

	UFUNCTION(Exec)
	void ToggleNightmareForm() const;

	UFUNCTION(Exec)
	void ToggleHUD() const;
	
	UFUNCTION(Exec)
	static void ForceCrash();

	UFUNCTION(Exec)
	static void ForceGarbageCollection();
};
