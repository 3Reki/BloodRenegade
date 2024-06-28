#pragma once


#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ImGuiSubsystem.generated.h"

class ABloodCharacter;
class ABloodController;

UCLASS()
class BLOODRENEGADE_API UImGuiSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	TWeakObjectPtr<ABloodController> PlayerController;
	TWeakObjectPtr<ABloodCharacter> PlayerCharacter;

#if !UE_BUILD_SHIPPING
	
	ABloodController* GetPlayerController();
	ABloodCharacter* GetPlayerCharacter();
	void MainCharacterMenu();
	void SoldierMenu();
	void MainMenuBar();
	void MainCharacterWindow();

	FDelegateHandle OnImguiSetInputHandle;
	
	void SetMainMenuActive(bool State);
	
	virtual void BeginDestroy() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual TStatId GetStatId() const override { return GetStatID(); }
	virtual void Tick(float DeltaTime) override;
#endif

};
