#include "AIManagerSubsystem.h"
#include "BloodRenegade/AI/Soldier.h"
#include "EngineUtils.h"
#include "../../Controller/BloodCharacter.h"
#include <limits.h>

void FindAllSoldiers(const UWorld* World, TMap<TWeakObjectPtr<ASoldier>, FSoldierInfos>& Out)
{
	for (TActorIterator<ASoldier> It(World); It; ++It)
	{
		Out.Add(*It, FSoldierInfos((*It)->GetActorLocation()));
	}
}

void UAIManagerSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	FindAllSoldiers(&InWorld, Soldiers);
	ABloodCharacter* BloodCharacter = Cast<ABloodCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (BloodCharacter) 
	{
		BloodCharacter->CanBeStrike.BindUObject(this, &UAIManagerSubsystem::SetCanStrikePlayer);
	}
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASoldier::StaticClass(), () _soldiers);

}

void UAIManagerSubsystem::AddSoldier(const TWeakObjectPtr<ASoldier> Soldier) 
{
	const FSoldierInfos Infos { Soldier->GetActorLocation(), false, false };
	Soldiers.Add(Soldier, Infos);
}

void UAIManagerSubsystem::ResetAllSoldiers()
{
	for (TTuple<TWeakObjectPtr<ASoldier>, FSoldierInfos> SoldierAndInfos : Soldiers)
	{
		SoldierAndInfos.Key->SetActorLocation(SoldierAndInfos.Value.InitialPosition);
		SoldierAndInfos.Key->Revive();
	}
}

void UAIManagerSubsystem::ResetSoldier(ASoldier* Soldier)
{
	check(Soldier && Soldiers.Contains(Soldier));
	
	Soldier->SetActorLocation(Soldiers[Soldier].InitialPosition);
	Soldier->Revive();
}

TSoftObjectPtr<ASoldier> UAIManagerSubsystem::FindAIByFitness(const TSoftObjectPtr<AActor> AlertActor) 
{
	TSoftObjectPtr<ASoldier> BestSoldier = nullptr;
	float Fitness = TNumericLimits<float>::Max();
	
	for (TTuple<TWeakObjectPtr<ASoldier>, FSoldierInfos> SoldierInfos : Soldiers) 
	{
		const ASoldier* Soldier = SoldierInfos.Key.Get();
		if (!Soldier) continue;
		
		const float TmpFit = Soldier->GetFitnessByRange(AlertActor->GetActorLocation());
		if (TmpFit < Fitness)
		{
			Fitness = TmpFit;
			BestSoldier = SoldierInfos.Key.Get();
		}
	}

	return BestSoldier;
}

void UAIManagerSubsystem::ChangeGoal(const FName GoalKey, ASoldier* Soldier) 
{
	check(Soldier);
	Soldier->actual_goal_map_key = GoalKey;
	Soldier->ChangePlan();
}

TArray<ASoldier*> UAIManagerSubsystem::GetAllSoldiersInRange(FVector Pos, float Range) 
{
	TArray<ASoldier*> soldiers = TArray<ASoldier*>();
	for (TTuple<TWeakObjectPtr<ASoldier>, FSoldierInfos> soldier : Soldiers) 
	{
		if (soldier.Key.IsValid()) 
		{
			if (FVector::DistSquared(soldier.Key->GetActorLocation(), Pos) < Range * Range) 
			{
				soldiers.Add(soldier.Key.Get());
			}
		}
	}
	return soldiers;
}

void UAIManagerSubsystem::SetDetectionIndicatorVisible(const bool Visible)
{
	IsDetectionIndicatorVisible = Visible;

	for (TTuple<TWeakObjectPtr<ASoldier>, FSoldierInfos> soldier : Soldiers) 
	{
		if (soldier.Key.IsValid()) 
		{
			soldier.Key->SetDetectionIndicatorVisible(Visible);
		}
	}
}

void UAIManagerSubsystem::SetCanStrikePlayer(bool b) 
{
	for (TTuple<TWeakObjectPtr<ASoldier>, FSoldierInfos> Soldier : Soldiers) 
	{
		if (Soldier.Key.IsValid()) 
		{
			Soldier.Key.Get()->SetCanStrikePlayer(b);
		}
	}
}

void UAIManagerSubsystem::SetSoldierIsAlerted(ASoldier* Soldier, const bool IsAlerted)
{
	Soldiers[Soldier].IsAlerted = IsAlerted;

	if (IsAlerted) 
	{
		if (OnAlertChangedDelegate.IsBound()) OnAlertChangedDelegate.Execute(true);
	}
	else 
	{
		bool PlayerVisible = false;
		for (TTuple<TWeakObjectPtr<ASoldier>, FSoldierInfos> Soldier : Soldiers) 
		{
			if (Soldier.Value.IsAlerted) PlayerVisible = true;
		}
		if (!PlayerVisible && OnAlertChangedDelegate.IsBound()) OnAlertChangedDelegate.Execute(false);
	}
}

void UAIManagerSubsystem::SetSoldierIsSuspicious(ASoldier* Soldier, const bool IsSuspicious)
{
	Soldiers[Soldier].IsSuspicious = IsSuspicious;

	if (IsSuspicious) 
	{
		if (OnSuspiciousChangedDelegate.IsBound()) OnSuspiciousChangedDelegate.Execute(true);
	}
	else 
	{
		bool PlayerVisible = false;
		for (TTuple<TWeakObjectPtr<ASoldier>, FSoldierInfos> Soldier : Soldiers) 
		{
			if (Soldier.Value.IsSuspicious) PlayerVisible = true;
		}
		if (!PlayerVisible && OnSuspiciousChangedDelegate.IsBound()) OnSuspiciousChangedDelegate.Execute(false);
	}
}
