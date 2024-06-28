#pragma once

#include "CoreMinimal.h"
#include "StateSoundActor.generated.h"

class UTextRenderComponent;
class UAkComponent;
class UAkAudioEvent;

UENUM(BlueprintType)
enum class EGameTheme
{
	One,
	Two
};

UCLASS()
class BLOODRENEGADE_API AStateSoundActor : public AActor
{
	GENERATED_BODY()
	
protected:
	AStateSoundActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere, Category="Variables", meta = (AllowPrivateAccess="true"))
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, Category="Variables", meta = (AllowPrivateAccess="true"))
	UAkComponent* AkComponent;	
	
	UPROPERTY(EditAnywhere, Category="Variables", meta = (AllowPrivateAccess="true"))
	UTextRenderComponent* Text;

	// UPROPERTY(EditAnywhere, Category="Variables", meta = (AllowPrivateAccess="true"))
	// TEnumAsByte<EPlayType> PlayType = EPlayType::BeginPlay;
	
	UAkAudioEvent* AudioEvent;
	EGameTheme GameTheme;

	float LastUpdate;
	
	void Play() const;
	void SwitchState();
};
