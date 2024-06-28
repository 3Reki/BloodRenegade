#include "BloodGameInstance.h"

#include "Kismet/GameplayStatics.h"

void UBloodGameInstance::LoadComplete(const float LoadTime, const FString& MapName)
{
	Super::LoadComplete(LoadTime, MapName);

	UWorld* World = GetWorld();

	// Force a camera update manually just in case the player has been moved and the camera did not yet update it's position
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	CameraManager->UpdateCamera(0.1f);

	// And then block the game thread until all subsequent levels are loaded. This will also bring up a loading screen by default
	// if not overridden by GEngine->BeginStreamingPauseDelegate and GEngine->EndStreamingPauseDelegate.
	GEngine->BlockTillLevelStreamingCompleted(World);
}
