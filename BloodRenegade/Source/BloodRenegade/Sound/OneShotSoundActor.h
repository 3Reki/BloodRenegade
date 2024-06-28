#pragma once

#include "CoreMinimal.h"
#include "AkAudioEvent.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Actor.h"
#include "OneShotSoundActor.generated.h"

UCLASS()
class BLOODRENEGADE_API AOneShotSoundActor : public AActor
{
	GENERATED_BODY()
	
protected:
	AOneShotSoundActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category="Variables", meta = (AllowPrivateAccess="true"))
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, Category="Variables", meta = (AllowPrivateAccess="true"))
	UAkComponent* AkComponent;	
	
	UPROPERTY(EditAnywhere, Category="Variables", meta = (AllowPrivateAccess="true"))
	UTextRenderComponent* Text;
	
	UAkAudioEvent* AudioEvent;

	int32 EventID;
	bool bIsPlaying;

	FOnAkPostEventCallback BindCallback;
	
	void Play();
	void Stop();

	UFUNCTION()
	void EndCallback(EAkCallbackType CallbackType, UAkCallbackInfo* CallbackInfo);
};
