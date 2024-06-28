#include "ParameterSoundActor.h"

#include "AkAudioEvent.h"
#include "AkGameplayStatics.h"

AParameterSoundActor::AParameterSoundActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Text = CreateDefaultSubobject<UTextRenderComponent>("Text");
	Text->SetupAttachment(Root);
	
	AkComponent = CreateDefaultSubobject<UAkComponent>("AkComponent");
	AkComponent->SetupAttachment(Root);
}

void AParameterSoundActor::BeginPlay()
{
	Super::BeginPlay();

	AudioEvent = AkComponent->AkAudioEvent;
	
	// if (!AudioEvent || PlayType == EPlayType::None) return;
	Play();
}

void AParameterSoundActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!AudioEvent) return;
	DrawDebugCircle(GetWorld(), GetActorLocation(), AkComponent->GetAttenuationRadius(), 64, FColor::Yellow,
		false, -1, 0, 2, FVector(1, 0, 0), FVector(0, 1, 0), false);
	
	if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (PlayerController->IsInputKeyDown(EKeys::E)) {
			PitchValue -= DeltaTime / MinToMaxDuration * 2;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Value : %f"), PitchValue));
		}
		else if (PlayerController->IsInputKeyDown(EKeys::R)) {
			PitchValue += DeltaTime / MinToMaxDuration * 2;
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Value : %f"), PitchValue));
		}
		PitchValue = FMath::Clamp(PitchValue, -1.f, 1.f);
		UAkGameplayStatics::SetRTPCValue(nullptr, PitchValue, 0, this, "MarioDuration");
	}
}

void AParameterSoundActor::Play() const
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Play "+AkComponent->AkAudioEvent->GetName());
	AkComponent->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
}
