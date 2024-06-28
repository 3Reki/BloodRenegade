#include "BloodCheatManager.h"

#include "BloodRenegade/ActorComponents/ClimbSystem.h"
#include "BloodRenegade/ActorComponents/HideSystem.h"
#include "BloodRenegade/Controller/BloodController.h"
#include "BloodRenegade/Controller/BloodCharacter.h"
#include "BloodRenegade/Controller/HealthSystem.h"
#include "BloodRenegade/Controller/BloodCharacterDataAsset.h"
#include "BloodRenegade/Controller/BloodHUD.h"
#include "BloodRenegade/Controller/BloodPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


void UBloodCheatManager::Ghost()
{
	const ABloodController* BloodController = Cast<ABloodController>(GetOuterAPlayerController());
	check(BloodController);
	
	APawn* Pawn = BloodController->GetPawn();
	if (!Pawn)
		return;

	ABloodCharacter* Character = Cast<ABloodCharacter>(Pawn);
	if (!Character)
		return;
	
	if (Character->GetCharacterMovement()->bCheatFlying)
	{
		Character->ClientCheatWalk();

		BloodController->RemoveMappingContext(BloodController->NoClipMappingContext);
	}
	else
	{
		Character->ClientCheatGhost();
		Character->ClimbSystem->TimelineCheckLedge.Stop();
		Character->WasGrounded = false;

		BloodController->AddMappingContext(BloodController->NoClipMappingContext, 10);
	}
}

void UBloodCheatManager::ToggleSkillsCooldown() const
{
	const ABloodController* BloodController = Cast<ABloodController>(GetOuterAPlayerController());
	check(BloodController);

	BloodController->ToggleSkillsCooldown();
}

void UBloodCheatManager::SetHP(const int Value) const
{
	const ABloodController* BloodController = Cast<ABloodController>(GetOuterAPlayerController());
	check(BloodController);

	const ABloodCharacter* BloodCharacter = BloodController->GetPawn<ABloodCharacter>();
	check(BloodCharacter);

	BloodCharacter->HealthSystem->SetHP(Value);
}

void UBloodCheatManager::SetMaxHP(const int Value) const
{
	const ABloodController* BloodController = Cast<ABloodController>(GetOuterAPlayerController());
	check(BloodController);

	const ABloodCharacter* BloodCharacter = BloodController->GetPawn<ABloodCharacter>();
	check(BloodCharacter);

	ABloodHUD* BloodHUD = BloodController->GetHUD<ABloodHUD>();
	check(BloodHUD);
	
	BloodCharacter->DataAsset->MaxHp = Value;
	UE_LOG(LogTemp, Log, TEXT("%d"), BloodCharacter->DataAsset->MaxHp);
	BloodHUD->InitHUD();
}

void UBloodCheatManager::SetFear(const float Value) const
{
	const ABloodController* BloodController = Cast<ABloodController>(GetOuterAPlayerController());
	check(BloodController);

	const ABloodCharacter* BloodCharacter = BloodController->GetPawn<ABloodCharacter>();
	check(BloodCharacter);
}

void UBloodCheatManager::ToggleNightmareForm() const
{
	const ABloodController* BloodController = Cast<ABloodController>(GetOuterAPlayerController());
	check(BloodController);

	ABloodCharacter* BloodCharacter = BloodController->GetPawn<ABloodCharacter>();
	check(BloodCharacter);
}

void UBloodCheatManager::ToggleHUD() const
{
	ABloodHUD* BloodHUD = GetOuterAPlayerController()->GetHUD<ABloodHUD>();
	check(BloodHUD);

	BloodHUD->SetActiveHUD(!BloodHUD->IsVisible);
}

void UBloodCheatManager::ForceCrash()
{
	// Let's definitely crash
	*((int*) 0) = 0;

	check( false );
}

void UBloodCheatManager::ForceGarbageCollection()
{
	GEngine->ForceGarbageCollection( true );
}
