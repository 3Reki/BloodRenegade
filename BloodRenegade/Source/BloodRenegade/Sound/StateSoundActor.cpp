#include "StateSoundActor.h"

#include "AkAudioEvent.h"
#include "AkComponent.h"
#include "AkGameplayStatics.h"
#include "Components/TextRenderComponent.h"

AStateSoundActor::AStateSoundActor()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Text = CreateDefaultSubobject<UTextRenderComponent>("Text");
	Text->SetupAttachment(Root);
	
	AkComponent = CreateDefaultSubobject<UAkComponent>("AkComponent");
	AkComponent->SetupAttachment(Root);
}

void AStateSoundActor::BeginPlay()
{
	Super::BeginPlay();

	AudioEvent = AkComponent->AkAudioEvent;
	
	// if (!AudioEvent || PlayType == EPlayType::None) return;
	Play();
}

void AStateSoundActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!AudioEvent) return;
	DrawDebugCircle(GetWorld(), GetActorLocation(), AkComponent->GetAttenuationRadius(), 64, FColor::Blue,
		false, -1, 0, 2, FVector(1, 0, 0), FVector(0, 1, 0), false);

	if (LastUpdate + 1 >= GetWorld()->GetTimeSeconds()) return;	
	
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (PlayerController->IsInputKeyDown(EKeys::SpaceBar)) SwitchState();
	}
}

void AStateSoundActor::Play() const
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Play "+AkComponent->AkAudioEvent->GetName()+ " with state One");
	AkComponent->PostAssociatedAkEvent(0, FOnAkPostEventCallback());
	UAkGameplayStatics::SetState(nullptr, "GameTheme", "One");
}

void AStateSoundActor::SwitchState()
{
	GameTheme = GameTheme == EGameTheme::One ? EGameTheme::Two : EGameTheme::One;
	FString NewState = GameTheme == EGameTheme::One ? "One" : "Two";
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Switch state to "+NewState);
	UAkGameplayStatics::SetState(nullptr, "GameTheme", FName(NewState));
	LastUpdate = GetWorld()->GetTimeSeconds();
}
