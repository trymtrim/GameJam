// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MainCharacterController.generated.h"

UCLASS()
class GAMEJAM_API AMainCharacterController : public ACharacter
{
	GENERATED_BODY ()

public:
	//Sets default values for this character's properties
	AMainCharacterController ();

	//Called every frame
	virtual void Tick (float DeltaTime) override;

	//Called to bind functionality to input
	virtual void SetupPlayerInputComponent (class UInputComponent* PlayerInputComponent) override;

protected:
	//Called when the game starts or when spawned
	virtual void BeginPlay () override;
};
