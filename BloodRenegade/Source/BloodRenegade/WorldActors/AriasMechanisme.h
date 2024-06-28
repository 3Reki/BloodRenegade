// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BloodRenegade/Interfaces/IInteractable.h"
#include "GameFramework/Actor.h"
#include "AriasMechanisme.generated.h"

class AAriaObjectif;
class USphereComponent;

UCLASS()
class BLOODRENEGADE_API AAriasMechanisme : public AActor, public IIInteractable
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void StartActiveMechanisme();

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<AAriaObjectif> Aria;

	AAriasMechanisme();

protected:

	UPROPERTY(EditAnywhere, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> Mesh;
};
