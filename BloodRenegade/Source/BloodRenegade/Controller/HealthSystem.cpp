// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthSystem.h"

#include "BloodCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "BloodRenegade/Controller/BloodPlayerState.h"
#include "BloodRenegade/Controller/BloodHUD.h"
#include "BloodRenegade/Controller/BloodCharacterDataAsset.h"

// Sets default values for this component's properties
UHealthSystem::UHealthSystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UHealthSystem::BeginPlay()
{
	Super::BeginPlay();
	CurrentOwner = GetOwner<ABloodCharacter>();
}

// Called every frame
void UHealthSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthSystem::TakeHit(int Dmg)
{
	if(CurrentOwner->CurrentPlayerState->CurrentHP <= 0) return;
	
	CurrentOwner->CurrentPlayerState->CurrentHP -= Dmg;
	CurrentOwner->SetCanBeDamaged(false);
	
	TWeakObjectPtr<ABloodCharacter> WeakThis = CurrentOwner;
	CurrentOwner->GetWorldTimerManager().SetTimer(InvincibleTimerHandle, FTimerDelegate::CreateWeakLambda(this, 
		   [WeakThis]
		   {
			   if(WeakThis.IsValid())
			   {
				   WeakThis->SetCanBeDamaged(true);
			   }
		   }), DataAsset->TimerInvincible, false);
	
	if(CurrentOwner->CurrentPlayerState->CurrentHP <= 0)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		CurrentOwner->PlayAnimMontage(DeathMontage);
		CurrentOwner->Death();
		CurrentOwner->GetWorldTimerManager().SetTimer(TimingDeath, this, &UHealthSystem::DeathReset, 3, false);
		return;
	}
}

void UHealthSystem::WaitForResetHP(float f)
{
	if(CurrentOwner->GetIsSeenBySoldier()) return;
	GetWorld()->GetTimerManager().ClearTimer(WaitResetTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(WaitResetTimerHandle, this, &UHealthSystem::ResetHP, f, false);
}

void UHealthSystem::ResetHP()
{
	if(CurrentOwner->CurrentPlayerState->CurrentHP == DataAsset->MaxHp) return;
	SetHP(DataAsset->MaxHp);
	CurrentOwner->TakeHitEffect(false);
}

void UHealthSystem::DeathReset()
{
	UGameplayStatics::OpenLevel(this, *GetWorld()->GetName(), false);
}


void UHealthSystem::SetHP(const int Value)
{
	TWeakObjectPtr<ABloodCharacter> playerCharacter = CurrentOwner;
	check(IsValid(playerCharacter.Get()));

	int* currentHP = &playerCharacter->CurrentPlayerState->CurrentHP;
	*currentHP = Value;

	if(*currentHP == playerCharacter->DataAsset->MaxHp) return;

	if(*currentHP == 0)
	{
		CurrentOwner->PlayAnimMontage(DeathMontage);
		CurrentOwner->Death();
		return;
	}
}


