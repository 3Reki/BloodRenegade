#include "OneShotSoundActor.h"

#include "AkComponent.h"

AOneShotSoundActor::AOneShotSoundActor()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = Root;

	Text = CreateDefaultSubobject<UTextRenderComponent>("Text");
	Text->SetupAttachment(Root);
	
	AkComponent = CreateDefaultSubobject<UAkComponent>("AkComponent");
	AkComponent->SetupAttachment(Root);
}

void AOneShotSoundActor::BeginPlay()
{
	Super::BeginPlay();

	AudioEvent = AkComponent->AkAudioEvent;
	if (!AudioEvent) return;

	static FName CBName("EndCallback");
	BindCallback.BindUFunction(this, CBName);
}

void AOneShotSoundActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!AudioEvent) return;
	DrawDebugCircle(GetWorld(), GetActorLocation(), AkComponent->GetAttenuationRadius(), 64, FColor::Red,
		false, -1, 0, 2, FVector(1, 0, 0), FVector(0, 1, 0), false);
	
	if (const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (PlayerController->IsInputKeyDown(EKeys::A)) Play();
		if (PlayerController->IsInputKeyDown(EKeys::Z)) Stop();
	}
}

void AOneShotSoundActor::Play()
{
	if (bIsPlaying) return;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Play "+AkComponent->AkAudioEvent->GetName());
	EventID = AkComponent->PostAssociatedAkEvent(AK_EndOfEvent, BindCallback);
	bIsPlaying = true;
}

void AOneShotSoundActor::Stop()
{
	if (!bIsPlaying) return;
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Stop "+AkComponent->AkAudioEvent->GetName());
	AudioEvent->ExecuteAction(AkActionOnEventType::Stop, GetOwner(), EventID, 0);
	bIsPlaying = false;
}

void AOneShotSoundActor::EndCallback(EAkCallbackType CallbackType, UAkCallbackInfo* CallbackInfo)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Callback "+AkComponent->AkAudioEvent->GetName());
	bIsPlaying = false;
}

