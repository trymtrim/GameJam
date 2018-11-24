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

	void HitPolymorph ();

	UPROPERTY (BlueprintReadOnly) FVector laserTargetPosition;
	UPROPERTY (Replicated, BlueprintReadOnly) float polymorphCharge = 0.0f;

	UPROPERTY (Replicated, BlueprintReadOnly) bool isPig = false;
	UPROPERTY (Replicated, BlueprintReadOnly) float pigTimer = 0.0f;

	UPROPERTY (Replicated, BlueprintReadOnly) bool showTargetPigTimer = false;
	UPROPERTY (Replicated, BlueprintReadOnly) float targetPigTimer = 0.0f;

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

	float _removeTargetPigTimerTimer = 0.0f;

	UCameraComponent* _cameraComponent;
};
