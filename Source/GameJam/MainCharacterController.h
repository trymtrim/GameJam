// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
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

	UPROPERTY (BlueprintReadOnly) FVector laserTargetPosition;

protected:
	//Called when the game starts or when spawned
	virtual void BeginPlay () override;

	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void TurnIntoPigBP ();
	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void TurnIntoHumanBP ();
	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void StartPolymorphBP ();
	UFUNCTION (BlueprintImplementableEvent, Category = "Character Controller")
	void StopPolymorphBP ();

private:
	UFUNCTION (Server, Reliable, WithValidation)
	void StartPolymorph ();
	UFUNCTION (Server, Reliable, WithValidation)
	void StopPolymorph ();

	void Polymorph ();

	void TurnIntoPig ();
	void TurnIntoHuman ();

	bool _isPolymorphing = false;

	UCameraComponent* _cameraComponent;
};
